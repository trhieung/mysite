#ifndef _CUSTOM_FILESYSTEM_H
#define _CUSTOM_FILESYSTEM_H

#include "global.h"

namespace custom_filesystem
{
    // Description of the module
    extern std::string desc;

    struct file_struct
    {
        std::string name;
        bool is_file;
        unsigned long long content_length;

        // Output stream operator overload for easy display
        friend std::ostream &operator<<(std::ostream &os, const file_struct &file)
        {
            os << std::left << std::setw(30) << file.name
               << std::left << std::setw(10) << (file.is_file ? "File" : "Folder")
               << std::left << std::setw(15) << file.content_length;
            return os;
        }
    };

    void notion();
    bool path_valid(const std::string &path);
    bool path_join_cwd(const std::string &relative_path, std::string &out_path);
    bool path_folder(const std::string &path); // Check if path is a folder
    bool path_file(const std::string &path);   // Check if path is a file

    bool folder_mkdir(const std::string &path);
    bool folder_infor(const std::string &path, std::vector<file_struct> &files);

    bool file_content_length(const std::string &path, unsigned long long &content_length);
    bool file_name(const std::string &path, std::string &file_name);

    void example_main(const int &argc, char *argv[]);

    void test_path_valid();
    void test_path_join_cwd();
    void test_path_folder(); // Check if path is a folder
    void test_path_file();   // Check if path is a file

    void test_folder_mkdir();
    void test_folder_infor();

    void test_file_content_length();
    void test_file_name();
}

// Definition of the module description
inline std::string custom_filesystem::desc = R"(
------------------------------ Notion from Example Module -------------------------------
|(+) Purpose:                                                                           |
|    (-) This module provides functionality for file and directory operations.          |
|                                                                                       |
|(+) Support Functions:                                                                 |
|                                                                                       |
|    |---------------------------------------------------------------------------|      |
|    | Function Name | Input (argv0: type, argv1: type)  | Output (return type)  |      |
|    |---------------|-----------------------------------|-----------------------|      |
|    | notion        | none                              | none                  |      |
|    | path_valid    | const std::string& path          | bool                  |      |
|    | path_join_cwd | const std::string& relative_path, | bool                  |      |
|    |               | std::string& out_path            |                       |      |
|    | folder_mkdir  | const std::string& path          | bool                  |      |
|    | file_content_length | const std::string& path, ull& content_length | bool  |      |
|    | file_name     | const std::string& path, std::string& file_name | bool      |      |
|    | example_main  | const int& argc, char* argv[]    | void                  |      |
|                                                                                       |
-----------------------------------------------------------------------------------------
)";

inline void custom_filesystem::notion()
{
    std::cout << desc;
}

inline bool custom_filesystem::path_valid(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

inline bool custom_filesystem::path_join_cwd(const std::string &relative_path, std::string &out_path)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr)
    {
        out_path = std::string(cwd) + "/" + relative_path;
        return true;
    }
    return false;
}

inline bool custom_filesystem::path_folder(const std::string &path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0)
    {
        return S_ISDIR(buffer.st_mode); // Check if path is a directory
    }
    return false; // Path does not exist
}

inline bool custom_filesystem::path_file(const std::string &path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0)
    {
        return S_ISREG(buffer.st_mode); // Check if path is a regular file
    }
    return false; // Path does not exist
}

inline bool custom_filesystem::folder_mkdir(const std::string &path)
{
    if (path_valid(path))
    {
        return true; // Folder already exists
    }
    return (mkdir(path.c_str(), 0777) == 0); // Attempt to create folder
}

// Function to gather information about the contents of a folder
inline bool custom_filesystem::folder_infor(const std::string &path, std::vector<custom_filesystem::file_struct> &files)
{
    if (!path_folder(path))
    {
        return false; // Not a valid folder
    }

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path.c_str())) == nullptr)
    {
        return false; // Failed to open directory
    }

    while ((entry = readdir(dir)) != nullptr)
    {
        // Skip the special entries "." and ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            file_struct file;
            file.name = entry->d_name;
            std::string full_path = path + "/" + file.name;

            // Determine if it's a file or a folder
            struct stat buffer;
            if (stat(full_path.c_str(), &buffer) == 0)
            {
                file.is_file = S_ISREG(buffer.st_mode);                  // Check if it's a file
                file.content_length = file.is_file ? buffer.st_size : 0; // Only set size for files
            }
            else
            {
                file.is_file = false; // If stat fails, assume it's not a file
                file.content_length = 0;
            }

            files.push_back(file); // Add file information to the vector
        }
    }
    closedir(dir);
    return true; // Successfully retrieved folder information
}

inline bool custom_filesystem::file_content_length(const std::string &path, unsigned long long &content_length)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode))
    {
        content_length = buffer.st_size;
        return true;
    }
    return false; // Not a file or does not exist
}

inline bool custom_filesystem::file_name(const std::string &path, std::string &file_name)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode))
    {
        size_t last_slash = path.find_last_of("/\\");
        file_name = path.substr(last_slash + 1);
        return true;
    }
    return false; // Not a file or does not exist
}

inline void custom_filesystem::example_main(const int &argc, char *argv[])
{
    test_path_valid();
    test_path_join_cwd();
    test_path_folder();
    test_path_file();

    test_folder_mkdir();
    test_folder_infor();

    test_file_content_length();
    test_file_name();
}

void custom_filesystem::test_path_valid()
{
    std::vector<std::string> paths = {"/home/nt/travis/template", "/home/nt/travis/template/", "huhu/", "huhu"};
    std::cout << "- Test path valid" << std::endl;
    for (const auto &path : paths)
    {
        std::cout << "Path: " << path << " Valid: " << (path_valid(path) ? "True" : "False") << std::endl;
    }
}

void custom_filesystem::test_path_join_cwd()
{
    std::vector<std::string> relative_paths = {"huhu/", "test.txt"};
    std::cout << "- Test path join cwd" << std::endl;
    for (const auto &relative : relative_paths)
    {
        std::string out_path;
        if (path_join_cwd(relative, out_path))
        {
            std::cout << "Joined path: " << out_path << std::endl;
        }
        else
        {
            std::cout << "Failed to join path: " << relative << std::endl;
        }
    }
}

void custom_filesystem::test_path_folder()
{
    std::vector<std::string> paths = {"/home/nt/travis/template", "/home/nt/travis/template/CMakeLists.txt", "/invalid/path"};
    std::cout << "- Test path folder" << std::endl;
    for (const auto &path : paths)
    {
        std::cout << "Path: " << path << " Is Folder: " << (path_folder(path) ? "True" : "False") << std::endl;
    }
}

void custom_filesystem::test_path_file()
{
    std::vector<std::string> paths = {"/home/nt/travis/template", "/home/nt/travis/template/CMakeLists.txt", "/invalid/path"};
    std::cout << "- Test path file" << std::endl;
    for (const auto &path : paths)
    {
        std::cout << "Path: " << path << " Is File: " << (path_file(path) ? "True" : "False") << std::endl;
    }
}

void custom_filesystem::test_folder_mkdir()
{
    std::vector<std::string> paths = {"huhu/test_folder1", "huhu/test_folder2", "/home/nt/travis/template/test/huhu", "/home/nt/travis/template/test/"};
    std::cout << "- Test folder mkdir" << std::endl;
    for (const auto &path : paths)
    {
        if (folder_mkdir(path))
        {
            std::cout << "Successfully created folder: " << path << std::endl;
        }
        else
        {
            std::cout << "Failed to create folder: " << path << std::endl;
        }
    }
}

// Function to test folder_infor
void custom_filesystem::test_folder_infor()
{
    std::vector<std::string> paths = {
        "/home/nt/travis/template/CMakeLists.txt",
        "/home/nt/travis/template/build",
        "/home/nt/travis/template/build/",
        "huhu"};

    std::cout << "- Test folder information" << std::endl;

    for (const auto &path : paths)
    {
        std::vector<file_struct> files;

        if (folder_infor(path, files))
        {
            std::cout << "Folder Information for: " << path << std::endl;
            for (const auto &file : files)
            {
                std::cout << file << std::endl; // Uses overloaded << operator
            }
        }
        else
        {
            std::cout << "Path is not a folder or does not exist: " << path << std::endl;
        }
    }
}

void custom_filesystem::test_file_content_length()
{
    std::vector<std::string> paths = {"/home/nt/travis/template/CMakeLists.txt", "/home/nt/travis/template", "huhu/", "huhu"};
    std::cout << "- Test file content length" << std::endl;
    for (const auto &path : paths)
    {
        unsigned long long length;
        if (file_content_length(path, length))
        {
            std::cout << "File: " << path << " Length: " << length << std::endl;
        }
        else
        {
            std::cout << "Cannot get length for: " << path << std::endl;
        }
    }
}

void custom_filesystem::test_file_name()
{
    std::vector<std::string> paths = {"/home/nt/travis/template/CMakeLists.txt", "huhu/", "huhu"};
    std::cout << "- Test file name" << std::endl;
    for (const auto &path : paths)
    {
        std::string file_name;
        if (custom_filesystem::file_name(path, file_name))
        {
            std::cout << "File: " << path << " Name: " << file_name << std::endl;
        }
        else
        {
            std::cout << "Cannot get name for: " << path << std::endl;
        }
    }
}


#endif // _CUSTOM_FILESYSTEM_H
