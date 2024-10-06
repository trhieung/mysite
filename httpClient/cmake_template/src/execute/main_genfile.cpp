#include <iostream>
#include <fstream>
#include <vector>
#include <getopt.h>
#include <cstdlib>
#include <sys/stat.h>
#include <string>

void showHelp(const char* progName) {
    std::cout << "Usage: " << progName << " [OPTIONS]\n"
              << "Generate a file of a specified size at a given path.\n\n"
              << "Options:\n"
              << "  -p, --path    Set directory path (default is current directory './')\n"
              << "  -s, --size    Set file size in bytes (default is 1MB)\n"
              << "  -e, --ext     Set file extension (default is .txt)\n"
              << "  -h, --help    Show this help message\n";
}

void generateFile(const std::string& filePath, std::size_t fileSize) {
    std::vector<char> buffer(fileSize, 0);
    std::ofstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file at " << filePath << std::endl;
        return;
    }

    file.write(buffer.data(), buffer.size());
    file.close();
    std::cout << "File created successfully at " << filePath << " with size " << fileSize << " bytes." << std::endl;
}

std::size_t parseSize(const std::string& sizeStr) {
    std::size_t size = std::stoull(sizeStr);
    if (sizeStr.back() == 'M' || sizeStr.back() == 'm') {
        size *= 1024 * 1024; // Convert from MB to bytes
    } else if (sizeStr.back() == 'K' || sizeStr.back() == 'k') {
        size *= 1024; // Convert from KB to bytes
    }
    return size;
}

std::string generateUniqueFileName(const std::string& dirPath, const std::string& extension) {
    std::string baseFileName = dirPath + "/genfile";
    std::string fileName = baseFileName + extension;
    int counter = 0;

    // Check for existing files and create a unique file name
    while (true) {
        struct stat buffer;
        if (stat(fileName.c_str(), &buffer) != 0) {
            break; // File does not exist
        }
        counter++;
        fileName = baseFileName + (counter > 0 ? std::to_string(counter) : "") + extension; // Append extension
    }

    return fileName;
}

bool directoryExists(const std::string& dirPath) {
    struct stat info;
    if (stat(dirPath.c_str(), &info) != 0) {
        return false; // Directory doesn't exist
    }
    return (info.st_mode & S_IFDIR) != 0; // Check if it's a directory
}

int main(int argc, char* argv[]) {
    std::string dirPath = "./"; // Default directory
    std::size_t fileSize = 1024 * 1024; // Default size (1MB)
    std::string extension = ".txt"; // Default file extension

    // Option structure for long options
    const struct option long_options[] = {
        {"path", required_argument, 0, 'p'},
        {"size", required_argument, 0, 's'},
        {"ext", required_argument, 0, 'e'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    bool sizeProvided = false; // Track if the size argument was provided

    while ((opt = getopt_long(argc, argv, "p:s:e:h", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'p':
                dirPath = optarg; // Set directory path
                break;
            case 's':
                fileSize = parseSize(optarg); // Parse file size
                sizeProvided = true; // Mark that size has been provided
                break;
            case 'e':
                extension = optarg; // Set file extension
                break;
            case 'h':
                showHelp(argv[0]); // Show help and exit
                return 0;
            default:
                showHelp(argv[0]);
                return 1;
        }
    }

    // Check if the specified directory exists
    if (!directoryExists(dirPath)) {
        std::cerr << "Error: Directory " << dirPath << " does not exist." << std::endl;
        return 1;
    }

    // Generate unique file name in the specified directory
    std::string filePath = generateUniqueFileName(dirPath, extension);

    // Use the default file size if -s flag was not provided
    if (!sizeProvided) {
        std::cout << "No size specified, defaulting to 1MB." << std::endl;
    }

    // Generate the file with the specified or default size
    generateFile(filePath, fileSize);
    return 0;
}
