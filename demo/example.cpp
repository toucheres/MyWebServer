#include <iostream>
#include "StringCrypto.h"

int main() {
    std::string originalText = "这是一个需要加密的字符串！";
    std::string key = "secret_key123";
    
    std::cout << "原始文本: " << originalText << std::endl;
    
    // 十六进制方式加密和解密
    std::string hexEncrypted = StringCrypto::encryptToHex(originalText, key);
    std::cout << "十六进制加密结果: " << hexEncrypted << std::endl;
    
    std::string hexDecrypted = StringCrypto::decryptFromHex(hexEncrypted, key);
    std::cout << "十六进制解密结果: " << hexDecrypted << std::endl;
    
    // Base64方式加密和解密
    std::string base64Encrypted = StringCrypto::encryptToBase64(originalText, key);
    std::cout << "Base64加密结果: " << base64Encrypted << std::endl;
    
    std::string base64Decrypted = StringCrypto::decryptFromBase64(base64Encrypted, key);
    std::cout << "Base64解密结果: " << base64Decrypted << std::endl;
    
    return 0;
}
