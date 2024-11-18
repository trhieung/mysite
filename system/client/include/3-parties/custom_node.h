#ifndef _CUSTOM_NODE_H_
#define _CUSTOM_NODE_H_

#include "global.h"
#include "custom_user.h"
#include "custom_filesystem.h"

namespace custom_node
{
    // UI variables
    std::string node_id = "3";
    std::string node_name = "Node3";
    std::string path_to_store = "../../storage_nodes/node3/";

    // Core functions
    void download(const std::string &start_byte, const std::string &filename);
    void run();

    // Test functions
    void api_upload(const std::string &filename = "user1_file1.txt");
    void api_download(const std::string &filename = "user1_file1.txt");
    void api_ready();
    bool api_init();

    void test_main();

    // Support functions
    // Helper function to split strings by a delimiter
    std::vector<std::string> split_string(const std::string &str, char delimiter);
}

inline void custom_node::download(const std::string &start_byte, const std::string &filename)
{
    try
    {
        // Construct the URL
        std::string url = "http://0.0.0.0:8080/api/node/" + node_id + "/download";

        // Create an instance of custom_curl for GET requests
        custom_curl curl_instance("get");

        // Prepare headers
        std::vector<std::string> headers = {
            "Filename: " + filename,
            "Range: bytes=" + start_byte + "-"};

        // Set the GET request with URL and headers
        curl_instance.set_get(url, headers);

        // Perform the GET request
        curl_instance.run_get();

        // Capture and handle the response
        std::string response = curl_instance.get_response();
        if (!response.empty())
        {
            // Determine the mode for writing to the file
            std::ios_base::openmode mode = std::ios::binary;
            if (start_byte == "0")
            {
                mode |= std::ios::trunc; // Overwrite if starting from byte 0
            }
            else
            {
                mode |= std::ios::app; // Append if continuing from a non-zero byte
            }

            // Save the file content to the storage path
            std::ofstream output_file(path_to_store + filename, mode);
            if (output_file)
            {
                output_file << response;
                std::cout << "Downloaded file saved to " << path_to_store + filename << std::endl;
            }
            else
            {
                std::cerr << "Error: Could not save downloaded file to " << path_to_store << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: No response received from server during file download." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred in download: " << e.what() << std::endl;
    }
}

inline void custom_node::run()
{
    try
    {
        // Step 1: Get response from the ready API
        std::string url = "http://0.0.0.0:8080/api/node/" + node_id + "/ready";

        // Create an instance of custom_curl for GET requests
        custom_curl curl_instance("get");

        // Perform the GET request
        curl_instance.set_get(url, {});
        curl_instance.run_get();

        // Capture the response
        std::string response = curl_instance.get_response();
        if (!response.empty())
        {
            // std::cout << "Node Ready Response: " << response << std::endl;

            // Step 2: Handle each request from the response
            // Response format: "download_user1_file1.txt\nupload_user2_file2.txt"
            std::vector<std::string> actions = split_string(response, '\n');

            for (const auto &action : actions)
            {
                std::vector<std::string> values = split_string(action, '/');
                // values = {action, begin byte, filename}
                if (values[0] == "download")
                {
                    download(values[1], values[2]);
                    // api_download(values[2]);
                }
                else if (values[0] == "upload")
                {
                    api_upload(values[2]);
                }
                else
                {
                    std::cerr << "Unknown action: " << action << std::endl;
                }
            }
        }
        else
        {
            std::cerr << "Error: No response received from server." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred in run: " << e.what() << std::endl;
    }
}

inline void custom_node::api_upload(const std::string &filename)
{
    try
    {
        // Construct file path and URL
        std::string path_to_file = path_to_store + filename;
        std::string url = "http://0.0.0.0:8080/api/node/" + node_id + "/upload";

        // Create an instance of custom_curl for POST requests
        custom_curl curl_instance("post");

        // Prepare headers
        std::vector<std::string> headers = {
            "Content-Type: application/octet-stream",
            "Filename: " + filename};

        // Read file content
        std::ifstream file_stream(path_to_file, std::ios::binary);
        if (!file_stream)
        {
            std::cerr << "Error: Could not open file " << path_to_file << " for upload." << std::endl;
            return;
        }

        std::ostringstream buffer;
        buffer << file_stream.rdbuf();
        std::string file_data = buffer.str();

        // Set the POST data with headers and file content
        curl_instance.set_post(url, file_data, headers);

        // Perform the POST request
        curl_instance.run_post();

        // Capture and handle the response
        std::string response = curl_instance.get_response();
        if (!response.empty())
        {
            std::cout << "Upload Response: " << response << std::endl;
        }
        else
        {
            std::cerr << "Error: No response received from server during file upload." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred in api_upload: " << e.what() << std::endl;
    }
}

inline void custom_node::api_download(const std::string &filename)
{
    try
    {
        // Construct the URL
        std::string url = "http://0.0.0.0:8080/api/node/" + node_id + "/download";

        // Create an instance of custom_curl for GET requests
        custom_curl curl_instance("get");

        // Prepare headers
        std::vector<std::string> headers = {
            "Filename: " + filename};

        // Set the GET request with URL and headers
        curl_instance.set_get(url, headers);

        // Perform the GET request
        curl_instance.run_get();

        // Capture and handle the response
        std::string response = curl_instance.get_response();
        if (!response.empty())
        {
            // Save the file content to the storage path
            std::ofstream output_file(path_to_store + filename, std::ios::binary);
            if (output_file)
            {
                output_file << response;
                std::cout << "Downloaded file saved to " << path_to_store + filename << std::endl;
            }
            else
            {
                std::cerr << "Error: Could not save downloaded file to " << path_to_store << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: No response received from server during file download." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred in api_download: " << e.what() << std::endl;
    }
}

inline void custom_node::api_ready()
{
    try
    {
        // Construct the URL
        std::string url = "http://0.0.0.0:8080/api/node/" + node_id + "/ready";

        // Create an instance of custom_curl for GET requests
        custom_curl curl_instance("get");

        // Set the GET request (no headers required)
        curl_instance.set_get(url, {});

        // Perform the GET request
        curl_instance.run_get();

        // Capture and handle the response
        std::string response = curl_instance.get_response();
        if (!response.empty())
        {
            std::cout << "Node Ready Response: " << response << std::endl;
        }
        else
        {
            std::cerr << "Error: No response received from server." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred in api_ready: " << e.what() << std::endl;
    }
}

inline bool custom_node::api_init()
{
    std::vector<custom_filesystem::file_struct> files;
    if (!custom_filesystem::folder_infor(path_to_store, files))
    {
        return false;
    }

    std::string file_data = "";
    for (const auto &file : files)
    {
        if (file.is_file){
            file_data += std::to_string(file.content_length) + "/" + file.name + "\n";
        }
    }

    try
    {
        // Construct URL
        std::string url = "http://0.0.0.0:8080/api/node/" + node_id + "/init";

        // Create an instance of custom_curl for POST requests
        custom_curl curl_instance("post");

        // Prepare headers
        std::string saved_path;
        custom_filesystem::path_join_cwd(path_to_store, saved_path);
        std::vector<std::string> headers = {
            "Content-Type: application/octet-stream",
            "Nodename: " + node_name,
            "PathToSave: " + saved_path};

        // Set the POST data with headers and file content
        curl_instance.set_post(url, file_data, headers);

        // Perform the POST request
        curl_instance.run_post();

        // Capture and handle the response
        std::string response = curl_instance.get_response();
        if (!response.empty())
        {
            std::cout << "Upload Response: " << response << std::endl;
        }
        else
        {
            std::cerr << "Error: No response received from server during file upload." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception occurred in api_upload: " << e.what() << std::endl;
    }

    return true;
}

// Support function to split strings by a delimiter
inline std::vector<std::string> custom_node::split_string(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream token_stream(str);
    std::string token;

    while (std::getline(token_stream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

inline void custom_node::test_main()
{
    // while (true)
    // {
    //     run();

    //     std::this_thread::sleep_for(std::chrono::seconds(5));
    // }
    api_init();
    // api_ready();
    // api_upload();
    // api_download();
}

#endif
