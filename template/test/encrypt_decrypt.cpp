#include <iostream>
#include <cstdlib>  // For system()
#include <fstream>  // For file operations

void encrypt(const std::string& inputFile, const std::string& outputFile, const std::string& password) {
    std::string encryptCommand = "openssl enc -aes-256-cbc -salt -in " + inputFile + " -out " + outputFile + " -pass pass:" + password;
    if (system(encryptCommand.c_str()) == 0) {
        std::cout << "Encryption successful. Encrypted file: " << outputFile << std::endl;
    } else {
        std::cerr << "Encryption failed." << std::endl;
    }
}

void decrypt(const std::string& inputFile, const std::string& outputFile, const std::string& password) {
    std::string decryptCommand = "openssl enc -d -aes-256-cbc -in " + inputFile + " -out " + outputFile + " -pass pass:" + password;
    if (system(decryptCommand.c_str()) == 0) {
        std::cout << "Decryption successful. Decrypted file: " << outputFile << std::endl;
    } else {
        std::cerr << "Decryption failed." << std::endl;
    }
}

int main() {
    std::string inputTextFile = "plaintext.txt";
    std::string encryptedFile = "encrypted.bin";
    std::string decryptedFile = "decrypted.txt";
    std::string password = "mysecret";  // You can set a password of your choice

    // Encrypt the text file to a binary file
    encrypt(inputTextFile, encryptedFile, password);

    // Decrypt the binary file back to a plaintext file
    decrypt(encryptedFile, decryptedFile, password);

    return 0;
}
