#include "webSocketFile.h"
#include "serverFile.h"
#include <algorithm>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <iostream>
#include <string>

// 初始化静态成员，自动注册WebSocket协议处理函数
bool WebSocketUtil::autoRegistered = WebSocketUtil::initialize();

// 初始化方法，注册WebSocket协议处理函数
bool WebSocketUtil::initialize() {
    return serverFile::registerProtocolHandler(Agreement::WebSocket, WebSocketUtil::wsEventloop);
}

// WebSocket帧操作
std::string WebSocketUtil::makeWebSocketFrame(bool fin, uint8_t opcode, const std::string& payload, bool masked)
{
    std::string frame;
    
    // 第一个字节: FIN位和操作码
    uint8_t byte1 = (fin ? 0x80 : 0x00) | (opcode & 0x0F);
    frame.push_back(byte1);
    
    // 第二个字节: MASK位和负载长度
    uint8_t byte2 = 0;
    if (masked) byte2 |= 0x80;
    
    // 设置长度
    size_t payload_len = payload.size();
    if (payload_len < 126) {
        byte2 |= payload_len;
        frame.push_back(byte2);
    } else if (payload_len <= 0xFFFF) {
        byte2 |= 126;
        frame.push_back(byte2);
        
        // 16位长度
        frame.push_back((payload_len >> 8) & 0xFF);
        frame.push_back(payload_len & 0xFF);
    } else {
        byte2 |= 127;
        frame.push_back(byte2);
        
        // 64位长度
        for (int i = 7; i >= 0; --i) {
            frame.push_back((payload_len >> (i * 8)) & 0xFF);
        }
    }
    
    // 如果有掩码，添加掩码键和掩码数据
    if (masked) {
        // 生成随机掩码键
        uint8_t mask_key[4];
        for (int i = 0; i < 4; ++i) {
            mask_key[i] = rand() % 256;
            frame.push_back(mask_key[i]);
        }
        
        // 添加掩码后的负载
        for (size_t i = 0; i < payload.size(); ++i) {
            frame.push_back(payload[i] ^ mask_key[i % 4]);
        }
    } else {
        // 添加原始负载
        frame.append(payload);
    }
    
    return frame;
}

std::string WebSocketUtil::parseWebSocketFrame(const std::string& frame)
{
    // 只实现基本解析，实际应用中需要完整处理
    if (frame.size() < 2) return "";
    
    // 读取第一个字节，获取FIN位和操作码
    uint8_t byte1 = static_cast<uint8_t>(frame[0]);
    bool fin = (byte1 & 0x80) != 0;
    uint8_t opcode = byte1 & 0x0F;
    
    // 读取第二个字节，获取MASK位和负载长度
    uint8_t byte2 = static_cast<uint8_t>(frame[1]);
    bool masked = (byte2 & 0x80) != 0;
    uint64_t payload_len = byte2 & 0x7F;
    
    // 处理扩展长度
    size_t header_size = 2;
    if (payload_len == 126) {
        if (frame.size() < 4) return "";
        payload_len = (static_cast<uint8_t>(frame[2]) << 8) | static_cast<uint8_t>(frame[3]);
        header_size = 4;
    } else if (payload_len == 127) {
        if (frame.size() < 10) return "";
        payload_len = 0;
        for (int i = 0; i < 8; ++i) {
            payload_len = (payload_len << 8) | static_cast<uint8_t>(frame[2 + i]);
        }
        header_size = 10;
    }
    
    // 掩码处理
    if (masked) {
        if (frame.size() < header_size + 4) return "";
        
        // 提取掩码键
        uint8_t mask_key[4];
        for (int i = 0; i < 4; ++i) {
            mask_key[i] = static_cast<uint8_t>(frame[header_size + i]);
        }
        header_size += 4;
        
        // 提取并解码负载
        std::string payload;
        for (size_t i = 0; i < payload_len; ++i) {
            if (header_size + i >= frame.size()) break;
            payload.push_back(frame[header_size + i] ^ mask_key[i % 4]);
        }
        return payload;
    } else {
        // 直接提取无掩码负载
        if (header_size + payload_len > frame.size()) {
            payload_len = frame.size() - header_size;
        }
        return frame.substr(header_size, payload_len);
    }
}

// Base64编码
std::string base64_encode(const unsigned char* input, size_t length) {
    BIO* bio;
    BIO* b64;
    BUF_MEM* bufferPtr;
    
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    
    // 不换行
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    
    BIO_write(bio, input, static_cast<int>(length));
    BIO_flush(bio);
    
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string result(bufferPtr->data, bufferPtr->length);
    
    BIO_free_all(bio);
    
    return result;
}

// WebSocket握手响应
std::string WebSocketUtil::makeWebSocketHandshake(const std::string& clientKey) {
    // 按照WebSocket规范，将客户端键与WebSocket GUID连接
    const char* guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string concat = clientKey + guid;
    
    // 计算SHA-1哈希
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(concat.c_str()), concat.length(), hash);
    
    // Base64编码
    std::string encoded = base64_encode(hash, SHA_DIGEST_LENGTH);
    
    // 构建HTTP响应
    std::string response = 
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " + encoded + "\r\n"
        "\r\n";
    
    return response;
}

// 判断是否应该升级到WebSocket
bool WebSocketUtil::shouldbeUpdataToWS(const serverFile& httpfile) {
    const auto& headers = httpfile.getContent();
    
    // 检查必要的头部
    auto connection = headers.find("connection");
    auto upgrade = headers.find("upgrade");
    auto key = headers.find("sec-websocket-key");
    auto version = headers.find("sec-websocket-version");
    
    if (connection == headers.end() || upgrade == headers.end() || 
        key == headers.end() || version == headers.end()) {
        return false;
    }
    
    // 检查头部值
    bool connectionUpgrade = false;
    std::string connectionValue = connection->second;
    std::transform(connectionValue.begin(), connectionValue.end(), connectionValue.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (connectionValue.find("upgrade") != std::string::npos) {
        connectionUpgrade = true;
    }
    
    std::string upgradeValue = upgrade->second;
    std::transform(upgradeValue.begin(), upgradeValue.end(), upgradeValue.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    return connectionUpgrade && upgradeValue == "websocket" && !key->second.empty();
}

// WebSocket事件循环
Task<void, void> WebSocketUtil::wsEventloop(serverFile* self) {
    while (self->fileState) {
        int ret = self->socketfile.eventGo();
        if (ret == -1 || 
            (!self->socketfile.handle.context) || 
            (self->socketfile.handle.context->fd_state == SocketFile::WRONG)) {
            self->fileState = false;
            co_yield {};
        }
        
        std::string_view data = self->socketfile.read_added();
        if (!data.empty()) {
            // 解析WebSocket帧
            std::string frameData(data.data(), data.size());
            std::string message = WebSocketUtil::parseWebSocketFrame(frameData);
            
            // 存储消息内容
            if (!message.empty()) {
                self->content["message"] = message;
                
                // 调用回调处理消息
                self->handle();
            }
        }
        
        co_yield {};
    }
    co_return;
}

// WebSocketResponse实现
WebSocketResponse::WebSocketResponse(uint8_t opcode, bool fin, bool masked)
    : opcode(opcode), fin(fin), masked(masked)
{
}

WebSocketResponse& WebSocketResponse::with_content(const std::string& new_content)
{
    content = new_content;
    return *this;
}

WebSocketResponse WebSocketResponse::text(const std::string& content)
{
    return WebSocketResponse(WebSocketUtil::TEXT).with_content(content);
}

WebSocketResponse WebSocketResponse::binary(const std::string& content)
{
    return WebSocketResponse(WebSocketUtil::BINARY).with_content(content);
}

WebSocketResponse WebSocketResponse::ping(const std::string& content)
{
    return WebSocketResponse(WebSocketUtil::PING).with_content(content);
}

WebSocketResponse WebSocketResponse::pong(const std::string& content)
{
    return WebSocketResponse(WebSocketUtil::PONG).with_content(content);
}

WebSocketResponse WebSocketResponse::close(uint16_t code, const std::string& reason)
{
    std::string payload;
    if (code > 0) {
        // 添加关闭代码（网络字节序）
        payload.push_back(static_cast<char>((code >> 8) & 0xFF));
        payload.push_back(static_cast<char>(code & 0xFF));
        // 添加关闭原因
        payload.append(reason);
    }
    return WebSocketResponse(WebSocketUtil::CLOSE).with_content(payload);
}

WebSocketResponse::operator std::string() const
{
    // 利用WebSocketUtil的createWebSocketFrame方法
    return WebSocketUtil::makeWebSocketFrame(fin, opcode, content, masked);
}