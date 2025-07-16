#include "webSocketMessage.h"
#include <random>
#include <algorithm>
#include <vector>

// WebSocketResponse实现
WebSocketResponse::WebSocketResponse(WebSocketOpcode opcode, bool fin, bool masked)
    : opcode_(opcode), fin_(fin), masked_(masked)
{
}

WebSocketResponse& WebSocketResponse::with_content(const std::string& new_content)
{
    content_ = new_content;
    return *this;
}

WebSocketResponse WebSocketResponse::text(const std::string& content)
{
    return WebSocketResponse(WebSocketOpcode::TEXT).with_content(content);
}

WebSocketResponse WebSocketResponse::binary(const std::string& content)
{
    return WebSocketResponse(WebSocketOpcode::BINARY).with_content(content);
}

WebSocketResponse WebSocketResponse::ping(const std::string& content)
{
    return WebSocketResponse(WebSocketOpcode::PING).with_content(content);
}

WebSocketResponse WebSocketResponse::pong(const std::string& content)
{
    return WebSocketResponse(WebSocketOpcode::PONG).with_content(content);
}

WebSocketResponse WebSocketResponse::close(uint16_t code, const std::string& reason)
{
    std::string payload;
    if (code > 0)
    {
        // 添加关闭代码（网络字节序）
        payload.push_back(static_cast<char>((code >> 8) & 0xFF));
        payload.push_back(static_cast<char>(code & 0xFF));
        // 添加关闭原因
        payload.append(reason);
    }
    return WebSocketResponse(WebSocketOpcode::CLOSE).with_content(payload);
}

WebSocketResponse::operator std::string() const
{
    return makeWebSocketFrame(content_, opcode_, fin_, masked_);
}

// 实现 WebSocketResponse::makeWebSocketFrame 静态方法
std::string WebSocketResponse::makeWebSocketFrame(const std::string& payload, WebSocketOpcode opcode, bool fin, bool masked)
{
    std::vector<uint8_t> frame;
    
    uint8_t first_byte = static_cast<uint8_t>(opcode);
    if (fin)
    {
        first_byte |= 0x80;
    }
    frame.push_back(first_byte);
    
    uint8_t second_byte = 0;
    if (masked)
    {
        second_byte |= 0x80;
    }
    
    size_t payload_length = payload.length();
    if (payload_length <= 125)
    {
        second_byte |= static_cast<uint8_t>(payload_length);
        frame.push_back(second_byte);
    }
    else if (payload_length <= 65535)
    {
        second_byte |= 126;
        frame.push_back(second_byte);
        
        frame.push_back(static_cast<uint8_t>((payload_length >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(payload_length & 0xFF));
    }
    else
    {
        second_byte |= 127;
        frame.push_back(second_byte);
        
        for (int i = 0; i < 4; ++i)
        {
            frame.push_back(0);
        }
        frame.push_back(static_cast<uint8_t>((payload_length >> 24) & 0xFF));
        frame.push_back(static_cast<uint8_t>((payload_length >> 16) & 0xFF));
        frame.push_back(static_cast<uint8_t>((payload_length >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(payload_length & 0xFF));
    }
    
    uint32_t mask_key = 0;
    if (masked)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dist;
        mask_key = dist(gen);
        
        frame.push_back(static_cast<uint8_t>((mask_key >> 24) & 0xFF));
        frame.push_back(static_cast<uint8_t>((mask_key >> 16) & 0xFF));
        frame.push_back(static_cast<uint8_t>((mask_key >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(mask_key & 0xFF));
    }
    
    if (!payload.empty())
    {
        std::vector<uint8_t> payload_bytes(payload.begin(), payload.end());
        
        if (masked)
        {
            uint8_t mask_bytes[4] = {
                static_cast<uint8_t>((mask_key >> 24) & 0xFF),
                static_cast<uint8_t>((mask_key >> 16) & 0xFF),
                static_cast<uint8_t>((mask_key >> 8) & 0xFF),
                static_cast<uint8_t>(mask_key & 0xFF)
            };
            
            for (size_t i = 0; i < payload_bytes.size(); ++i)
            {
                payload_bytes[i] ^= mask_bytes[i % 4];
            }
        }
        
        frame.insert(frame.end(), payload_bytes.begin(), payload_bytes.end());
    }
    
    return std::string(frame.begin(), frame.end());
}
WebSocketRequst WebSocketRequst::close(uint16_t code, const std::string& reason)
{
    std::string payload;
    payload.push_back(static_cast<char>((code >> 8) & 0xFF));
    payload.push_back(static_cast<char>(code & 0xFF));
    payload.append(reason);

    return WebSocketRequst(WebSocketOpcode::CLOSE).withContent(payload);
}

WebSocketRequst::operator std::string() const
{
    return toString();
}

std::string WebSocketRequst::toString() const
{
    return WebSocketRequst::makeWebSocketFrame(content_, opcode_, fin_, masked_);
}
std::string WebSocketRequst::makeWebSocketFrame(const std::string& payload,
                                                WebSocketOpcode opcode, bool fin,
                                                bool masked)
{
    return WebSocketResponse::makeWebSocketFrame(payload, opcode, fin, masked);
}
// WebSocketMessage实现
WebSocketRequst::WebSocketRequst(WebSocketOpcode opcode, bool fin, bool masked)
    : opcode_(opcode), fin_(fin), masked_(masked), mask_key_(0)
{
    if (masked)
    {
        mask_key_ = generateMaskKey();
    }
}
uint32_t WebSocketRequst::generateMaskKey()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;
    return dis(gen);
}

WebSocketRequst& WebSocketRequst::withContent(const std::string& content)
{
    content_ = content;
    return *this;
}

WebSocketRequst& WebSocketRequst::withMaskKey(uint32_t key)
{
    mask_key_ = key;
    return *this;
}

WebSocketRequst WebSocketRequst::text(const std::string& content)
{
    return WebSocketRequst(WebSocketOpcode::TEXT).withContent(content);
}

WebSocketRequst WebSocketRequst::binary(const std::string& content)
{
    return WebSocketRequst(WebSocketOpcode::BINARY).withContent(content);
}

WebSocketRequst WebSocketRequst::ping(const std::string& content)
{
    return WebSocketRequst(WebSocketOpcode::PING).withContent(content);
}

WebSocketRequst WebSocketRequst::pong(const std::string& content)
{
    return WebSocketRequst(WebSocketOpcode::PONG).withContent(content);
}

// 实现 WebSocketRequst::parseWebSocketFrame 静态方法
std::pair<std::string, WebSocketOpcode> WebSocketRequst::parseWebSocketFrame(const std::string& frame, bool& fin)
{
    if (frame.length() < 2)
    {
        return {"", WebSocketOpcode::CLOSE};
    }

    uint8_t first_byte = static_cast<uint8_t>(frame[0]);
    fin = (first_byte & 0x80) != 0;
    WebSocketOpcode opcode = static_cast<WebSocketOpcode>(first_byte & 0x0F);

    uint8_t second_byte = static_cast<uint8_t>(frame[1]);
    bool masked = (second_byte & 0x80) != 0;
    size_t payload_length = second_byte & 0x7F;

    size_t header_size = 2;

    if (payload_length == 126)
    {
        if (frame.length() < 4)
        {
            return {"", WebSocketOpcode::CLOSE};
        }
        payload_length = static_cast<uint16_t>(static_cast<uint8_t>(frame[2]) << 8 |
                                               static_cast<uint8_t>(frame[3]));
        header_size += 2;
    }
    else if (payload_length == 127)
    {
        if (frame.length() < 10)
        {
            return {"", WebSocketOpcode::CLOSE};
        }

        payload_length = 0;
        for (int i = 6; i < 10; ++i)
        {
            payload_length = (payload_length << 8) | static_cast<uint8_t>(frame[i]);
        }
        header_size += 8;
    }

    uint32_t mask_key = 0;
    if (masked)
    {
        if (frame.length() < header_size + 4)
        {
            return {"", WebSocketOpcode::CLOSE};
        }

        mask_key = (static_cast<uint32_t>(static_cast<uint8_t>(frame[header_size])) << 24) |
                   (static_cast<uint32_t>(static_cast<uint8_t>(frame[header_size + 1])) << 16) |
                   (static_cast<uint32_t>(static_cast<uint8_t>(frame[header_size + 2])) << 8) |
                   static_cast<uint32_t>(static_cast<uint8_t>(frame[header_size + 3]));
        header_size += 4;
    }

    if (frame.length() < header_size + payload_length)
    {
        return {"", WebSocketOpcode::CLOSE};
    }

    std::string payload = frame.substr(header_size, payload_length);

    if (masked && !payload.empty())
    {
        std::vector<uint8_t> payload_vec(payload.begin(), payload.end());
        uint8_t mask_bytes[4] = {
            static_cast<uint8_t>(mask_key >> 24),
            static_cast<uint8_t>((mask_key >> 16) & 0xFF),
            static_cast<uint8_t>((mask_key >> 8) & 0xFF),
            static_cast<uint8_t>(mask_key & 0xFF)
        };

        for (size_t i = 0; i < payload_vec.size(); ++i)
        {
            payload_vec[i] ^= mask_bytes[i % 4];
        }

        payload = std::string(payload_vec.begin(), payload_vec.end());
    }

    return {payload, opcode};
}