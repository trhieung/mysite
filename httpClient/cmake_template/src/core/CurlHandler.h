#ifndef _CURL_HANDLER_H
#define _CURL_HANDLER_H
#include <iostream>
#include <string>
#include <getopt.h>
#include <atomic>
#include <vector>
#include <fstream>
#include <iomanip>
#include <regex>
#include <mutex>
#include <memory>
#include <sstream>
#include <limits>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "CurlData.h"

#define debug true

// Class declarations
class CommandHandler;
class CurlHandler;

// Class to handle Curl tasks
class CurlHandler
{
public:
    CurlHandler();

    void add_download_task(const unsigned short id, const std::string &url, const std::string &des_folder);

    void start_downloads();

    void show_all_tasks();
    void stop_all_tasks();

    void pause_task(const unsigned short &id);
    void resume_task(const unsigned short &id);
    void delete_task(const unsigned short &id);

    friend class CommandHandler;

private:
    struct http_response_head_short
    {
        std::string status;
        std::string content_length;
    };
    struct http_response
    {
        // general headers
        std::string status;
        std::string connection;

        // response headers
        std::string accept_ranges;

        // representation headers
        std::string content_type;
    };

    std::mutex _mutex;
    std::vector<std::unique_ptr<CurlData>> curlsData;
    std::vector<std::thread> curlsThread;

    unsigned short num_activate_download = 0;

    void end_downloads();
    void download_task(void *tdata);
    bool head_request(const int &_socfd, http_response_head_short &_res_head_data);

    bool check_url_format(const std::string &url);
    bool _save_stoi(const std::string &num, int &result);
};

// FUNCTION IMPLEMENTATIONS

CurlHandler::CurlHandler()
    : num_activate_download(0)
{
}

void CurlHandler::add_download_task(const unsigned short id, const std::string &url, const std::string &des_folder)
{
    if (!check_url_format(url))
    {
        std::cout << "Invalid URL format." << std::endl;
        return;
    }

    curlsData.emplace_back(std::make_unique<CurlData>(id, url, des_folder));

    std::cout << "Download task added: ID=" << id << ", URL=" << url << ", Destination=" << des_folder << std::endl;
}

void CurlHandler::start_downloads()
{
    unsigned short sz = curlsData.size();
    while (num_activate_download < sz)
    {
        // Create a new thread for each download task
        curlsThread.emplace_back(&CurlHandler::download_task, this, curlsData[num_activate_download].get());
        ++num_activate_download; // Increment the active download count
    }

    this->end_downloads(); // Wait for all downloads to finish
}

void CurlHandler::end_downloads()
{
    unsigned short sz = curlsThread.size();
    for (unsigned short i = 0; i < sz; ++i)
    {
        if (curlsThread[i].joinable())
        {
            curlsThread[i].join(); // Ensure each thread completes its task
        }
    }
    std::cout << "All downloads completed." << std::endl;
}

void CurlHandler::download_task(void *tdata)
{
    CurlData *curl_data = static_cast<CurlData *>(tdata);

    // Extract the address (ip/hostname, port) and print initial progress
    std::pair<std::string, std::string> addr = curl_data->get_address();
    std::string ip_or_hostname = addr.first;
    std::string port_str = addr.second;
    int port = 80; // Default HTTP port

    // Try to convert port to integer if provided
    if (!port_str.empty() && !_save_stoi(port_str, port))
    {
        std::cerr << "Failed to extract port number, defaulting to 80." << std::endl;
    }

    // Step 1: Create the socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }

    // Step 2: Resolve hostname using gethostbyname or use IP directly
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_or_hostname.c_str(), &server_addr.sin_addr) <= 0)
    {
        // IP address interpretation failed, trying to resolve hostname
        struct hostent *host = gethostbyname(ip_or_hostname.c_str());
        if (host == nullptr)
        {
            std::cerr << "Error resolving hostname: " << ip_or_hostname << std::endl;
            close(sockfd);
            return;
        }
        // Copy resolved IP address into server_addr
        memcpy(&server_addr.sin_addr, host->h_addr_list[0], host->h_length);
    }

    // Step 3: Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Failed to connect to " << ip_or_hostname << ":" << port << std::endl;
        close(sockfd);
        return;
    }

    std::cout << "Connected to " << ip_or_hostname << " on port " << port << std::endl;

    // Step 4: Send HEAD request and handle response
    http_response_head_short res_head_data;
    if (head_request(sockfd, res_head_data))
    {

        // Handle the response here if necessary
    }
    else
    {
        std::cerr << "Failed to send request or receive response." << std::endl;
        close(sockfd);
        return;
    }

    // Step 5: Close the connection
    close(sockfd);
    std::cout << "Connection closed." << std::endl;

    // Continue download progress with curl_data->print_progress_bar
    // curl_data->print_progress_bar(_mutex);
}

bool CurlHandler::head_request(const int &sockfd, http_response_head_short &_res_head_data)
{
    // Construct the HTTP HEAD request
    std::string request = "HEAD / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";

    // Step 1: Send HTTP HEAD request
    ssize_t sent_bytes = send(sockfd, request.c_str(), request.size(), 0);
    if (sent_bytes < 0)
    {
        std::cerr << "Failed to send request." << std::endl;
        return false;
    }

    // Step 2: Receive response
    char buffer[1024];
    ssize_t received_bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received_bytes < 0)
    {
        std::cerr << "Failed to receive response." << std::endl;
        return false;
    }

    buffer[received_bytes] = '\0'; // Null-terminate the response
    std::string response(buffer);

    // Step 3: Parse the HTTP response (basic parsing for status and content-length)
    std::regex status_regex(R"(HTTP\/\d+\.\d+\s+(\d+)\s+)");
    std::regex content_length_regex(R"(Content-Length:\s*(\d+))");
    std::smatch match;

    if (std::regex_search(response, match, status_regex))
    {
        _res_head_data.status = match[1];
    }

    if (std::regex_search(response, match, content_length_regex))
    {
        _res_head_data.content_length = match[1];
    }

    std::cout << "HTTP Status: " << _res_head_data.status << std::endl;
    std::cout << "Content-Length: " << _res_head_data.content_length << std::endl;

    return true;
}

void CurlHandler::show_all_tasks()
{
    std::cout << "Current Download Tasks:" << std::endl;
    for (const auto &curl_data : curlsData)
    {
        curl_data->print_progress_bar(_mutex);
    }
}

void CurlHandler::stop_all_tasks()
{
    for (auto &curl_data : curlsData)
    {
        curl_data->is_stop = true; // Mark each task as stopped
    }
    std::cout << "All tasks have been marked to stop." << std::endl;
}

bool CurlHandler::check_url_format(const std::string &url)
{
    /*
    Simple regex check for URL format (basic validation)
    format: - http://<domain_name>:<port>/path
            - http://<domain_name>/path
            - http://<ip>:<port>/path
            - http://<ip>/path
    ex:
            - http://127.0.0.1:8080/file1
            - http://localhost:8080/file2
            - http://localhost/file3
    */

    const std::regex url_regex(R"(http:\/\/([a-zA-Z0-9\-\.]+|(\d{1,3}\.){3}\d{1,3})(:\d+)?(\/[^\s]*)?)");

    return std::regex_match(url, url_regex);
}

void CurlHandler::pause_task(const unsigned short &id)
{
    // Implementation to pause tasks
    std::cout << "Pausing tasks not yet implemented." << std::endl;
}

void CurlHandler::resume_task(const unsigned short &id)
{
    // Implementation to resume tasks
    std::cout << "Resuming tasks not yet implemented." << std::endl;
}

void CurlHandler::delete_task(const unsigned short &id)
{
    // Implementation to delete tasks
    std::cout << "Deleting tasks not yet implemented." << std::endl;
}

bool CurlHandler::_save_stoi(const std::string &num, int &result)
{
    try
    {
        result = std::stoi(num);
        return true; // Successful conversion
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid argument: cannot convert '" << num << "' to an integer." << std::endl;
        return false;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Out of range: '" << num << "' is too large or small for an integer." << std::endl;
        return false;
    }
}

#endif