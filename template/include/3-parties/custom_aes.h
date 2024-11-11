#ifndef _CUSTOM_AES_H
#define _CUSTOM_AES_H

#include "global.h"
#include <unordered_map>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>

#include <cstring>
#include <iostream>

namespace custom_aes
{
    // Use a fixed block size for AES-128, AES-192, AES-256.
    constexpr unsigned short _AES_BLOCK_SIZE = AES_BLOCK_SIZE;
    std::string key_string = "mykey";

    unsigned char _key_16[16] = {0}; // AES-128
    unsigned char _key_24[24] = {0}; // AES-192
    unsigned char _key_32[32] = {0}; // AES-256

    AES_KEY encryptKey128, encryptKey192, encryptKey256;

    // Key map to map AES key sizes to AES_KEY objects
    std::unordered_map<int, std::pair<const unsigned char *, AES_KEY *>> _key = {
        {128, std::make_pair(_key_16, &encryptKey128)},
        {192, std::make_pair(_key_24, &encryptKey192)},
        {256, std::make_pair(_key_32, &encryptKey256)}};

    // Function to generate AES key from a string
    void gen_key(const std::string &my_key);

    // Core AES encryption and decryption functions
    void encrypt_basic(char *plainText, char *cipherText, const unsigned short &textSize,
                       int key, const std::pair<const unsigned char *, AES_KEY *> &aes_key,
                       const unsigned short &blockSize = _AES_BLOCK_SIZE);

    void decrypt_basic(char *cipherText, char *plainText, const unsigned short &textSize,
                       int key, const std::pair<const unsigned char *, AES_KEY *> &aes_key,
                       const unsigned short &blockSize = _AES_BLOCK_SIZE);

    // Test AES functions
    void test_main();

    // Padding functions for AES
    unsigned long long padding_PKCS7(const std::string &plainText, char *&_plainText, const unsigned short &blocksize = _AES_BLOCK_SIZE);
    void remove_padding(char *plainText, size_t &size);
};

// init

// Generate the AES key from a given string
inline void custom_aes::gen_key(const std::string &my_key)
{
    unsigned char hash[32]; // SHA-256 produces a 256-bit (32-byte) hash

    // Initialize EVP_MD_CTX to create a SHA-256 hash from the provided key string
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr)
    {
        std::cerr << "Error initializing EVP_MD_CTX" << std::endl;
        return;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1)
    {
        std::cerr << "Error initializing digest" << std::endl;
        EVP_MD_CTX_free(mdctx);
        return;
    }

    if (EVP_DigestUpdate(mdctx, my_key.c_str(), my_key.length()) != 1)
    {
        std::cerr << "Error updating digest" << std::endl;
        EVP_MD_CTX_free(mdctx);
        return;
    }

    unsigned int len = 0;
    if (EVP_DigestFinal_ex(mdctx, hash, &len) != 1)
    {
        std::cerr << "Error finalizing digest" << std::endl;
        EVP_MD_CTX_free(mdctx);
        return;
    }

    EVP_MD_CTX_free(mdctx);

    // Extract the required portion of the hash depending on the key size
    if (_key.find(128) != _key.end())
    {
        memcpy(_key_16, hash, 16);
        AES_set_encrypt_key(_key_16, 128, &encryptKey128);
    }
    if (_key.find(192) != _key.end())
    {
        memcpy(_key_24, hash, 24);
        AES_set_encrypt_key(_key_24, 192, &encryptKey192);
    }
    if (_key.find(256) != _key.end())
    {
        memcpy(_key_32, hash, 32);
        AES_set_encrypt_key(_key_32, 256, &encryptKey256);
    }
}

// AES encryption function
inline void custom_aes::encrypt_basic(char *plainText, char *cipherText, const unsigned short &textSize,
                                      int key, const std::pair<const unsigned char *, AES_KEY *> &aes_key,
                                      const unsigned short &blockSize)
{
    AES_KEY *aes_encrypt_key = aes_key.second;
    for (unsigned short i = 0; i < textSize; i += blockSize)
    {
        AES_encrypt(reinterpret_cast<const unsigned char *>(plainText + i), reinterpret_cast<unsigned char *>(cipherText + i), aes_encrypt_key);
    }
}

// AES decryption function
inline void custom_aes::decrypt_basic(char *cipherText, char *plainText, const unsigned short &textSize,
                                      int key, const std::pair<const unsigned char *, AES_KEY *> &aes_key,
                                      const unsigned short &blockSize)
{
    AES_KEY *aes_decrypt_key = aes_key.second;
    for (unsigned short i = 0; i < textSize; i += blockSize)
    {
        AES_decrypt(reinterpret_cast<const unsigned char *>(cipherText + i), reinterpret_cast<unsigned char *>(plainText + i), aes_decrypt_key);
    }
}

// Testing AES encryption and decryption
inline void custom_aes::test_main()
{
    std::string init_key = "testkey";
    std::string plaintext = "hi from custom aes";

    custom_aes::gen_key(init_key);

    int key_size = 128; // For example, 128-bit key
    std::pair<const unsigned char *, AES_KEY *> _aes_key = custom_aes::_key[key_size];

    // Allocate memory for padded plaintext
    char *_plaintext;
    unsigned long long _size = custom_aes::padding_PKCS7(plaintext, _plaintext, AES_BLOCK_SIZE);

    // Allocate memory for encrypted and decrypted text
    char *encrypt_text = new char[_size];
    char *decrypt_text = new char[_size];

    // Encrypt and decrypt the data using the default key
    custom_aes::encrypt_basic(_plaintext, encrypt_text, _size, key_size, _aes_key, custom_aes::_AES_BLOCK_SIZE);
    custom_aes::decrypt_basic(encrypt_text, decrypt_text, _size, key_size, _aes_key, custom_aes::_AES_BLOCK_SIZE);

    // Remove padding after decryption
    custom_aes::remove_padding(decrypt_text, _size);

    // Print results
    std::cout << "Original plaintext: " << plaintext << std::endl;
    std::cout << "Encrypted text (hex): ";
    for (size_t i = 0; i < _size; ++i)
    {
        printf("%02x", (unsigned char)encrypt_text[i]);
    }
    std::cout << std::endl;

    std::cout << "Decrypted text: " << std::string(decrypt_text, _size) << std::endl;

    // Cleanup
    delete[] _plaintext;
    delete[] encrypt_text;
    delete[] decrypt_text;
}


// PKCS7 padding
inline unsigned long long custom_aes::padding_PKCS7(const std::string &plainText, char *&_plainText, const unsigned short &blocksize)
{
    size_t plainSize = plainText.size();
    size_t paddingLength = blocksize - (plainSize % blocksize);
    size_t paddedSize = plainSize + paddingLength;

    _plainText = new char[paddedSize];
    std::memcpy(_plainText, plainText.c_str(), plainSize);

    // Add padding bytes
    for (size_t i = plainSize; i < paddedSize; ++i)
    {
        _plainText[i] = paddingLength;
    }

    return paddedSize;
}

// Remove PKCS7 padding
inline void custom_aes::remove_padding(char *plainText, size_t &size)
{
    unsigned char paddingLength = plainText[size - 1];
    size -= paddingLength;  // Remove padding
    plainText[size] = '\0'; // Null terminate
}

#endif
