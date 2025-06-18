#ifndef STRING_CRYPTO_H
#define STRING_CRYPTO_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdexcept>

class StringCrypto {
public:
    // XOR加密，使用十六进制表示结果
    static std::string encryptToHex(const std::string& input, const std::string& key) {
        if (key.empty()) {
            throw std::invalid_argument("密钥不能为空");
        }

        std::stringstream ss;
        size_t keyLen = key.length();
        
        for (size_t i = 0; i < input.length(); i++) {
            // 使用循环密钥进行XOR
            char encrypted = input[i] ^ key[i % keyLen];
            // 转换为十六进制
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)encrypted;
        }
        
        return ss.str();
    }
    
    // 从十六进制解密
    static std::string decryptFromHex(const std::string& hexInput, const std::string& key) {
        if (key.empty()) {
            throw std::invalid_argument("密钥不能为空");
        }
        if (hexInput.length() % 2 != 0) {
            throw std::invalid_argument("十六进制字符串长度必须为偶数");
        }
        
        std::string result;
        size_t keyLen = key.length();
        
        for (size_t i = 0; i < hexInput.length(); i += 2) {
            // 从十六进制字符串获取字符
            std::string byteString = hexInput.substr(i, 2);
            char byte = (char)std::stoi(byteString, nullptr, 16);
            
            // XOR解密
            result += byte ^ key[(i/2) % keyLen];
        }
        
        return result;
    }
    
    // Base64编码表
    static const std::string base64Chars;
    
    // 将字符串转换为Base64编码
    static std::string toBase64(const std::string& input) {
        std::string ret;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        int i = 0;
        int j = 0;
        int in_len = input.size();
        
        while (in_len--) {
            char_array_3[i++] = input[j++];
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++)
                    ret += base64Chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; j < i + 1; j++)
                ret += base64Chars[char_array_4[j]];

            while (i++ < 3)
                ret += '=';
        }
        
        return ret;
    }
    
    // 从Base64解码
    static std::string fromBase64(const std::string& encoded) {
        int in_len = encoded.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::string ret;

        while (in_len-- && (encoded[in_] != '=') && isBase64(encoded[in_])) {
            char_array_4[i++] = encoded[in_]; in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64Chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; i < 3; i++)
                    ret += char_array_3[i];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;

            for (j = 0; j < 4; j++)
                char_array_4[j] = base64Chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; j < i - 1; j++)
                ret += char_array_3[j];
        }

        return ret;
    }
    
    // 使用Base64编码XOR加密结果
    static std::string encryptToBase64(const std::string& input, const std::string& key) {
        if (key.empty()) {
            throw std::invalid_argument("密钥不能为空");
        }
        
        std::string encrypted;
        size_t keyLen = key.length();
        
        for (size_t i = 0; i < input.length(); i++) {
            encrypted += input[i] ^ key[i % keyLen];
        }
        
        return toBase64(encrypted);
    }
    
    // 解密Base64编码的加密字符串
    static std::string decryptFromBase64(const std::string& base64Input, const std::string& key) {
        if (key.empty()) {
            throw std::invalid_argument("密钥不能为空");
        }
        
        std::string decoded = fromBase64(base64Input);
        std::string decrypted;
        size_t keyLen = key.length();
        
        for (size_t i = 0; i < decoded.length(); i++) {
            decrypted += decoded[i] ^ key[i % keyLen];
        }
        
        return decrypted;
    }
    
private:
    static bool isBase64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }
};

// 静态成员初始化
const std::string StringCrypto::base64Chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#endif // STRING_CRYPTO_H
