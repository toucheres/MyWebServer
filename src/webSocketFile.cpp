#include <openssl/evp.h>
#include <string>
#include <webSocketFile.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <array>

// 简单的WebSocket帧创建函数
std::string WebSocketUtil::createWebSocketFrame(bool fin, uint8_t opcode, const std::string& payload,
                                 bool masked)
{
    std::string frame;

    // 第一个字节: FIN(1位) + RSV1-3(3位) + Opcode(4位)
    uint8_t first_byte = opcode & 0x0F;
    if (fin)
    {
        first_byte |= 0x80;
    }
    frame.push_back(static_cast<char>(first_byte));

    // 第二个字节: MASK(1位) + Payload长度(7位)
    uint8_t second_byte = 0;
    if (masked)
    {
        second_byte |= 0x80;
    }

    // 处理payload长度
    if (payload.size() < 126)
    {
        second_byte |= payload.size();
        frame.push_back(static_cast<char>(second_byte));
    }
    else if (payload.size() <= 0xFFFF)
    {
        second_byte |= 126;
        frame.push_back(static_cast<char>(second_byte));
        frame.push_back(static_cast<char>((payload.size() >> 8) & 0xFF));
        frame.push_back(static_cast<char>(payload.size() & 0xFF));
    }
    else
    {
        second_byte |= 127;
        frame.push_back(static_cast<char>(second_byte));

        // 64位长度
        for (int i = 7; i >= 0; --i)
        {
            frame.push_back(static_cast<char>((payload.size() >> (i * 8)) & 0xFF));
        }
    }

    // 添加payload
    frame.append(payload);

    return frame;
}

// 解析WebSocket帧的简单实现
std::string WebSocketUtil::parseWebSocketFrame(const std::string& frame)
{
    if (frame.empty())
    {
        return "";
    }

    // 检查是否有足够的数据
    if (frame.size() < 2)
    {
        return "";
    }

    // 获取帧头信息
    bool fin = (frame[0] & 0x80) != 0;
    uint8_t opcode = frame[0] & 0x0F;
    bool masked = (frame[1] & 0x80) != 0;
    size_t payload_len = frame[1] & 0x7F;

    size_t header_size = 2;

    // 扩展长度
    if (payload_len == 126)
    {
        if (frame.size() < 4)
        {
            return "";
        }
        payload_len = ((static_cast<uint16_t>(frame[2]) << 8) | static_cast<uint8_t>(frame[3]));
        header_size = 4;
    }
    else if (payload_len == 127)
    {
        if (frame.size() < 10)
        {
            return "";
        }
        payload_len = 0;
        for (int i = 0; i < 8; ++i)
        {
            payload_len = (payload_len << 8) | static_cast<uint8_t>(frame[2 + i]);
        }
        header_size = 10;
    }

    // 掩码
    std::array<uint8_t, 4> mask_key = {0, 0, 0, 0};
    if (masked)
    {
        if (frame.size() < header_size + 4)
        {
            return "";
        }
        for (int i = 0; i < 4; ++i)
        {
            mask_key[i] = static_cast<uint8_t>(frame[header_size + i]);
        }
        header_size += 4;
    }

    // 检查是否有完整的payload
    if (frame.size() < header_size + payload_len)
    {
        return "";
    }

    // 解析payload
    std::string payload;
    payload.reserve(payload_len);

    for (size_t i = 0; i < payload_len; ++i)
    {
        char byte = frame[header_size + i];
        if (masked)
        {
            byte ^= mask_key[i % 4];
        }
        payload.push_back(byte);
    }

    return payload;
}

// 创建WebSocket握手响应的方法
std::string WebSocketUtil::createWebSocketHandshake(const std::string& clientKey)
{
    // 将key与魔术字符串连接
    std::string combined = clientKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    // 计算SHA1哈希
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()),
         combined.length(), hash);

    // Base64编码
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, hash, SHA_DIGEST_LENGTH);
    BIO_flush(b64);

    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);
    std::string acceptKey(bptr->data, bptr->length);
    BIO_free_all(b64);
    
    // 构建WebSocket握手响应
    std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                           "Upgrade: websocket\r\n"
                           "Connection: Upgrade\r\n"
                           "Sec-WebSocket-Accept: " +
                           acceptKey + "\r\n\r\n";
    
    return response;
}