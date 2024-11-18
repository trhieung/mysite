#ifndef _CUSTOM_USER_H_
#define _CUSTOM_USER_H_

#include <curl/curl.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

class custom_curl
{
private:
    struct curl_post
    {
        std::string url;
        std::string data;
        std::vector<std::string> headers;
        std::string response_data; // Store the response data
    };

    struct curl_get
    {
        std::string url;
        std::vector<std::string> headers;
        std::string response_data; // Store the response data
    };

    CURL *curl_api;
    std::string method;
    curl_post post_data;
    curl_get get_data;

    // Write callback function for capturing response
    static size_t writeCallBack(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t totalSize = size * nmemb;
        std::string *response = static_cast<std::string *>(userp);
        response->append(static_cast<char *>(contents), totalSize);
        return totalSize;
    }

public:
    custom_curl(const std::string &method = "post");
    ~custom_curl();

    void set_post(const std::string &url, const std::string &data, const std::vector<std::string> &headers);
    void set_get(const std::string &url, const std::vector<std::string> &headers);
    bool run_post();
    bool run_get();
    std::string get_response() const;
};

// Constructor
inline custom_curl::custom_curl(const std::string &method) : curl_api(curl_easy_init()), method(method)
{
    if (!curl_api)
    {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

// Destructor
inline custom_curl::~custom_curl()
{
    if (curl_api)
    {
        curl_easy_cleanup(curl_api);
    }
}

// Set POST data
inline void custom_curl::set_post(const std::string &url, const std::string &data, const std::vector<std::string> &headers)
{
    if (method != "post")
    {
        throw std::runtime_error("Method mismatch: Expected POST");
    }

    post_data.url = url;
    post_data.data = data;
    post_data.headers = headers;
}

// Set GET data
inline void custom_curl::set_get(const std::string &url, const std::vector<std::string> &headers)
{
    if (method != "get")
    {
        throw std::runtime_error("Method mismatch: Expected GET");
    }

    get_data.url = url;
    get_data.headers = headers;
}

// Run POST request
inline bool custom_curl::run_post()
{
    if (method != "post")
    {
        throw std::runtime_error("Method mismatch: Expected POST");
    }

    curl_easy_setopt(curl_api, CURLOPT_URL, post_data.url.c_str());
    curl_easy_setopt(curl_api, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_api, CURLOPT_POSTFIELDS, post_data.data.c_str());

    struct curl_slist *header_list = nullptr;
    for (const auto &header : post_data.headers)
    {
        header_list = curl_slist_append(header_list, header.c_str());
    }
    curl_easy_setopt(curl_api, CURLOPT_HTTPHEADER, header_list);

    post_data.response_data.clear();
    curl_easy_setopt(curl_api, CURLOPT_WRITEFUNCTION, writeCallBack);
    curl_easy_setopt(curl_api, CURLOPT_WRITEDATA, &post_data.response_data);

    CURLcode res = curl_easy_perform(curl_api);

    if (header_list)
    {
        curl_slist_free_all(header_list);
    }

    return (res == CURLE_OK); // Return true if successful
}

// Run GET request
inline bool custom_curl::run_get()
{
    if (method != "get")
    {
        throw std::runtime_error("Method mismatch: Expected GET");
    }

    curl_easy_setopt(curl_api, CURLOPT_URL, get_data.url.c_str());
    curl_easy_setopt(curl_api, CURLOPT_HTTPGET, 1L);

    struct curl_slist *header_list = nullptr;
    for (const auto &header : get_data.headers)
    {
        header_list = curl_slist_append(header_list, header.c_str());
    }
    curl_easy_setopt(curl_api, CURLOPT_HTTPHEADER, header_list);

    get_data.response_data.clear();
    curl_easy_setopt(curl_api, CURLOPT_WRITEFUNCTION, writeCallBack);
    curl_easy_setopt(curl_api, CURLOPT_WRITEDATA, &get_data.response_data);

    CURLcode res = curl_easy_perform(curl_api);

    if (header_list)
    {
        curl_slist_free_all(header_list);
    }

    return (res == CURLE_OK); // Return true if successful
}

// Get the captured response
inline std::string custom_curl::get_response() const
{
    if (method == "post")
    {
        return post_data.response_data;
    }
    else if (method == "get")
    {
        return get_data.response_data;
    }
    return "";
}
namespace custom_user
{
    std::string user_id = "1";
    std::string node_id = "1";

    // Core API functions
    void api_download_get(const std::string &filename = "file1.txt");
    void api_download_post(const std::string &filename = "file1.txt");
    void api_files();
    void api_upload(const std::string &file_path = "../../storage_users/user1/file1.txt");
    void api_ready();
    void api_login();

    void test_main();
}

// Definitions
inline void custom_user::api_download_get(const std::string &filename)
{
    try
    {
        std::string url = "http://0.0.0.0:8080/api/user-node/" + user_id + "-" + node_id + "/download";

        custom_curl curl_instance("get");
        std::vector<std::string> headers = {
            "Filename: " + filename
        };

        curl_instance.set_get(url, headers);

        if (curl_instance.run_get()) // Check the return value of run_get
        {
            std::string response = curl_instance.get_response();
            if (!response.empty())
            {
                std::cout << "Download GET Response: " << response << std::endl;
            }
            else
            {
                std::cerr << "Error: Empty response from server during GET download." << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: GET request failed for " << url << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in api_download_get: " << e.what() << std::endl;
    }
}

inline void custom_user::api_download_post(const std::string &filename)
{
    try
    {
        std::string url = "http://0.0.0.0:8080/api/user-node/" + user_id + "-" + node_id + "/download";
        std::string payload = "{\"file_name\": \"" + filename + "\", \"file_range\": \"bytes=?-?\"}";

        custom_curl curl_instance("post");
        std::vector<std::string> headers = {
            "Content-Type: application/json"
        };

        curl_instance.set_post(url, payload, headers);

        if (curl_instance.run_post()) // Check the return value of run_post
        {
            std::string response = curl_instance.get_response();
            if (!response.empty())
            {
                std::cout << "Download POST Response: " << response << std::endl;
            }
            else
            {
                std::cerr << "Error: Empty response from server during POST download." << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: POST request failed for " << url << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in api_download_post: " << e.what() << std::endl;
    }
}

inline void custom_user::api_files()
{
    try
    {
        custom_curl curl_instance("get");
        curl_instance.set_get("http://0.0.0.0:8080/api/user-node/1-1/files",
                              {"Authorization: Bearer your_token_here"});

        if (curl_instance.run_get())
        {
            std::cout << "GET Response: " << curl_instance.get_response() << std::endl;
        }
        else
        {
            std::cerr << "Error: GET request failed for files endpoint." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in api_files: " << e.what() << std::endl;
    }
}

inline void custom_user::api_upload(const std::string &file_path)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file: " << file_path << std::endl;
        return;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string file_content = buffer.str();
    file.close();

    custom_curl curl_instance("post");
    std::string url = "http://0.0.0.0:8080/api/user-node/" + user_id + "-" + node_id + "/upload";
    std::vector<std::string> headers = {
        "Content-Type: application/octet-stream",
        "Filename: " + file_path.substr(file_path.find_last_of("/\\") + 1),
        "Range: bytes=0-" + std::to_string(file_content.size() - 1)
    };

    curl_instance.set_post(url, file_content, headers);

    if (curl_instance.run_post())
    {
        std::cout << "Upload Response: " << curl_instance.get_response() << std::endl;
    }
    else
    {
        std::cerr << "Error: POST request failed during file upload." << std::endl;
    }
}

inline void custom_user::api_ready()
{
    try
    {
        std::string url = "http://0.0.0.0:8080/api/user-node/" + user_id + "-" + node_id + "/ready";

        custom_curl curl_instance("get");

        if (curl_instance.run_get())
        {
            std::cout << "Ready Response: " << curl_instance.get_response() << std::endl;
        }
        else
        {
            std::cerr << "Error: GET request failed for ready endpoint." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in api_ready: " << e.what() << std::endl;
    }
}

inline void custom_user::api_login()
{
    try
    {
        custom_curl curl_instance("post");
        curl_instance.set_post(
            "http://0.0.0.0:8080/api/user/login",
            R"({"username": "user1", "password": "pass1"})",
            {"Content-Type: application/json"});

        if (curl_instance.run_post())
        {
            std::cout << "Login Response: " << curl_instance.get_response() << std::endl;
        }
        else
        {
            std::cerr << "Error: POST request failed for login endpoint." << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in api_login: " << e.what() << std::endl;
    }
}

inline void custom_user::test_main()
{
    // api_login();
    // api_ready();
    api_upload();
    // api_files();
    api_download_post();
    // api_download_get();
}

#endif
