#include "webSocketClient.h"
#include "httpServerFile.h"
#include <algorithm>
#include <iomanip>
#include <random>
#include <sstream>
#include <openssl/sha.h>
#include <openssl/evp.h>

// WebSocketFrame 实现
std::vector<uint8_t> WebSocketFrame::serialize() const
{
    std::vector<uint8_t> result;
    
    // 第一个字节：FIN + RSV + Opcode
    uint8_t first_byte = (fin ? 0x80 : 0x00) | static_cast<uint8_t>(opcode);
    result.push_back(first_byte);
    
    // 第二个字节：MASK + Payload length
    uint8_t second_byte = masked ? 0x80 : 0x00;
    
    if (payload_length < 126)
    {
        second_byte |= static_cast<uint8_t>(payload_length);
        result.push_back(second_byte);
    }
    else if (payload_length < 65536)
    {
        second_byte |= 126;
        result.push_back(second_byte);
        result.push_back(static_cast<uint8_t>(payload_length >> 8));
        result.push_back(static_cast<uint8_t>(payload_length & 0xFF));
    }
    else
    {
        second_byte |= 127;
        result.push_back(second_byte);
        for (int i = 7; i >= 0; --i)
        {
            result.push_back(static_cast<uint8_t>((payload_length >> (i * 8)) & 0xFF));
        }
    }
    
    // Mask key (如果启用masking)
    if (masked)
    {
        result.push_back(static_cast<uint8_t>(mask_key >> 24));
        result.push_back(static_cast<uint8_t>((mask_key >> 16) & 0xFF));
        result.push_back(static_cast<uint8_t>((mask_key >> 8) & 0xFF));
        result.push_back(static_cast<uint8_t>(mask_key & 0xFF));
    }
    
    // Payload
    if (!payload.empty())
    {
        std::vector<uint8_t> masked_payload = payload;
        if (masked)
        {
            WebSocketClientUtil::maskData(masked_payload, mask_key);
        }
        result.insert(result.end(), masked_payload.begin(), masked_payload.end());
    }
    
    return result;
}

WebSocketFrame WebSocketFrame::deserialize(const std::vector<uint8_t>& data, size_t& offset)
{
    WebSocketFrame frame;
    
    if (data.size() - offset < 2)
    {
        throw std::runtime_error("Incomplete WebSocket frame");
    }
    
    // 解析第一个字节
    uint8_t first_byte = data[offset++];
    frame.fin = (first_byte & 0x80) != 0;
    frame.opcode = static_cast<WebSocketOpcode>(first_byte & 0x0F);
    
    // 解析第二个字节
    uint8_t second_byte = data[offset++];
    frame.masked = (second_byte & 0x80) != 0;
    uint8_t payload_len = second_byte & 0x7F;
    
    // 解析payload长度
    if (payload_len < 126)
    {
        frame.payload_length = payload_len;
    }
    else if (payload_len == 126)
    {
        if (data.size() - offset < 2)
        {
            throw std::runtime_error("Incomplete WebSocket frame length");
        }
        frame.payload_length = (data[offset] << 8) | data[offset + 1];
        offset += 2;
    }
    else
    {
        if (data.size() - offset < 8)
        {
            throw std::runtime_error("Incomplete WebSocket frame length");
        }
        frame.payload_length = 0;
        for (int i = 0; i < 8; ++i)
        {
            frame.payload_length = (frame.payload_length << 8) | data[offset + i];
        }
        offset += 8;
    }
    
    // 解析mask key
    if (frame.masked)
    {
        if (data.size() - offset < 4)
        {
            throw std::runtime_error("Incomplete WebSocket frame mask");
        }
        frame.mask_key = (data[offset] << 24) | (data[offset + 1] << 16) | 
                        (data[offset + 2] << 8) | data[offset + 3];
        offset += 4;
    }
    
    // 解析payload
    if (frame.payload_length > 0)
    {
        if (data.size() - offset < frame.payload_length)
        {
            throw std::runtime_error("Incomplete WebSocket frame payload");
        }
        
        frame.payload.assign(data.begin() + offset, data.begin() + offset + frame.payload_length);
        
        if (frame.masked)
        {
            WebSocketClientUtil::maskData(frame.payload, frame.mask_key);
        }
        
        offset += frame.payload_length;
    }
    
    return frame;
}

// WebSocketClient 实现
WebSocketClient::WebSocketClient(const std::string& ip, port port, const std::string& path)
    : target_port(port), target_ip(ip), target_path(path), 
      coru(WebSocketClientUtil::webSocketEventloop(this)),
      client_socket(SocketFile::createTcpClient(port, ip))
{
    sec_websocket_key_ = generateWebSocketKey();
    expected_accept_ = calculateExpectedAccept(sec_websocket_key_);
}

WebSocketClient::WebSocketClient(SocketFile&& socket, const std::string& path)
    : target_path(path), coru(WebSocketClientUtil::webSocketEventloop(this)),
      client_socket(std::move(socket))
{
    sec_websocket_key_ = generateWebSocketKey();
    expected_accept_ = calculateExpectedAccept(sec_websocket_key_);
    state_ = WebSocketClientState::HANDSHAKING;
}

WebSocketClient::WebSocketClient(WebSocketClient&& other)
    : target_port(other.target_port), target_ip(std::move(other.target_ip)),
      target_path(std::move(other.target_path)), headers_(std::move(other.headers_)),
      coru(std::move(other.coru)), state_(other.state_),
      sec_websocket_key_(std::move(other.sec_websocket_key_)),
      expected_accept_(std::move(other.expected_accept_)),
      client_socket(std::move(other.client_socket)),
      on_open_(std::move(other.on_open_)), on_message_(std::move(other.on_message_)),
      on_binary_(std::move(other.on_binary_)), on_close_(std::move(other.on_close_)),
      on_error_(std::move(other.on_error_)), on_ping_(std::move(other.on_ping_)),
      on_pong_(std::move(other.on_pong_))
{
    other.target_port = (port)-1;
    other.state_ = WebSocketClientState::CLOSED;
}

bool WebSocketClient::connect()
{
    if (state_ != WebSocketClientState::CONNECTING)
    {
        return false;
    }
    
    if (client_socket.getSocketStatus() == SocketStatus::OK)
    {
        state_ = WebSocketClientState::HANDSHAKING;
        return sendHandshakeRequest();
    }
    
    return false;
}

void WebSocketClient::close(WebSocketCloseCode code, const std::string& reason)
{
    if (state_ == WebSocketClientState::CONNECTED)
    {
        state_ = WebSocketClientState::CLOSING;
        
        WebSocketFrame close_frame;
        close_frame.opcode = WebSocketOpcode::CLOSE;
        close_frame.masked = true;
        close_frame.mask_key = WebSocketClientUtil::generateMaskKey();
        
        // 构造关闭帧的payload
        std::vector<uint8_t> payload;
        uint16_t close_code = static_cast<uint16_t>(code);
        payload.push_back(static_cast<uint8_t>(close_code >> 8));
        payload.push_back(static_cast<uint8_t>(close_code & 0xFF));
        
        if (!reason.empty())
        {
            payload.insert(payload.end(), reason.begin(), reason.end());
        }
        
        close_frame.payload = payload;
        close_frame.payload_length = payload.size();
        
        sendFrame(close_frame);
    }
    
    state_ = WebSocketClientState::CLOSED;
    client_socket.closeIt();
}

bool WebSocketClient::sendText(const std::string& message)
{
    if (state_ != WebSocketClientState::CONNECTED)
    {
        return false;
    }
    
    WebSocketFrame frame;
    frame.opcode = WebSocketOpcode::TEXT;
    frame.masked = true;
    frame.mask_key = WebSocketClientUtil::generateMaskKey();
    frame.payload.assign(message.begin(), message.end());
    frame.payload_length = frame.payload.size();
    
    return sendFrame(frame);
}

bool WebSocketClient::sendBinary(const std::vector<uint8_t>& data)
{
    if (state_ != WebSocketClientState::CONNECTED)
    {
        return false;
    }
    
    WebSocketFrame frame;
    frame.opcode = WebSocketOpcode::BINARY;
    frame.masked = true;
    frame.mask_key = WebSocketClientUtil::generateMaskKey();
    frame.payload = data;
    frame.payload_length = data.size();
    
    return sendFrame(frame);
}

bool WebSocketClient::sendPing(const std::vector<uint8_t>& data)
{
    if (state_ != WebSocketClientState::CONNECTED)
    {
        return false;
    }
    
    WebSocketFrame frame;
    frame.opcode = WebSocketOpcode::PING;
    frame.masked = true;
    frame.mask_key = WebSocketClientUtil::generateMaskKey();
    frame.payload = data;
    frame.payload_length = data.size();
    
    return sendFrame(frame);
}

bool WebSocketClient::sendPong(const std::vector<uint8_t>& data)
{
    if (state_ != WebSocketClientState::CONNECTED)
    {
        return false;
    }
    
    WebSocketFrame frame;
    frame.opcode = WebSocketOpcode::PONG;
    frame.masked = true;
    frame.mask_key = WebSocketClientUtil::generateMaskKey();
    frame.payload = data;
    frame.payload_length = data.size();
    
    return sendFrame(frame);
}

EventStatus WebSocketClient::eventGo()
{
    client_socket.eventGo();
    if (!coru.done())
    {
        coru.resume();
        return EventStatus::Continue;
    }
    return EventStatus::Stop;
}

std::string WebSocketClient::generateWebSocketKey()
{
    return WebSocketClientUtil::generateWebSocketKey();
}

std::string WebSocketClient::calculateExpectedAccept(const std::string& key)
{
    return WebSocketClientUtil::calculateWebSocketAccept(key);
}

bool WebSocketClient::sendHandshakeRequest()
{
    std::ostringstream request;
    request << "GET " << target_path << " HTTP/1.1\r\n";
    request << "Host: " << target_ip << ":" << (int)target_port << "\r\n";
    request << "Upgrade: websocket\r\n";
    request << "Connection: Upgrade\r\n";
    request << "Sec-WebSocket-Key: " << sec_websocket_key_ << "\r\n";
    request << "Sec-WebSocket-Version: 13\r\n";
    
    // 添加自定义头部
    for (const auto& header : headers_)
    {
        request << header.first << ": " << header.second << "\r\n";
    }
    
    request << "\r\n";
    
    client_socket.writeFile(request.str());
    return true;
}

bool WebSocketClient::sendFrame(const WebSocketFrame& frame)
{
    std::vector<uint8_t> frame_data = frame.serialize();
    std::string frame_str(frame_data.begin(), frame_data.end());
    client_socket.writeFile(frame_str);
    return true;
}

// WebSocketClientUtil 实现
Task<void, void> WebSocketClientUtil::webSocketEventloop(WebSocketClient* self)
{
    // ...existing code...
    co_return;
}

std::string WebSocketClientUtil::generateWebSocketKey()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    std::vector<uint8_t> key_bytes(16);
    for (auto& byte : key_bytes)
    {
        byte = dis(gen);
    }
    
    // Base64编码
    std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -6;
    
    for (uint8_t c : key_bytes)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            result.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6)
    {
        result.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    
    while (result.size() % 4)
    {
        result.push_back('=');
    }
    
    return result;
}

std::string WebSocketClientUtil::calculateWebSocketAccept(const std::string& key)
{
    std::string magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string combined = key + magic_string;
    
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()), combined.length(), hash);
    
    // Base64编码SHA1哈希
    std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -6;
    
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
    {
        val = (val << 8) + hash[i];
        valb += 8;
        while (valb >= 0)
        {
            result.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6)
    {
        result.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    
    while (result.size() % 4)
    {
        result.push_back('=');
    }
    
    return result;
}

uint32_t WebSocketClientUtil::generateMaskKey()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;
    return dis(gen);
}

void WebSocketClientUtil::maskData(std::vector<uint8_t>& data, uint32_t mask)
{
    uint8_t mask_bytes[4] = {
        static_cast<uint8_t>(mask >> 24),
        static_cast<uint8_t>((mask >> 16) & 0xFF),
        static_cast<uint8_t>((mask >> 8) & 0xFF),
        static_cast<uint8_t>(mask & 0xFF)
    };
    
    for (size_t i = 0; i < data.size(); ++i)
    {
        data[i] ^= mask_bytes[i % 4];
    }
}