#include <curl/curl.h>
#include <curl/easy.h>

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

#include "indicators.hpp"

// https://curl.se/libcurl/c/CURLOPT_XFERINFOFUNCTION.html
int download_progress_callback(void* clientp, curl_off_t dltotal,
                               curl_off_t dlnow, curl_off_t ultotal,
                               curl_off_t ulnow)
{
    indicators::ProgressBar* progress_bar =
        static_cast<indicators::ProgressBar*>(clientp);

    if (progress_bar->is_completed())
    {
        ;
    }
    else if (dltotal == 0)
    {
        progress_bar->set_progress(0);
    }
    else
    {
        int percentage =
            static_cast<float>(dlnow) / static_cast<float>(dltotal) * 100;
        progress_bar->set_progress(percentage);
    }

    // If your callback function returns CURL_PROGRESSFUNC_CONTINUE it will
    // cause libcurl to continue executing the default progress function. return
    // CURL_PROGRESSFUNC_CONTINUE;

    return 0;
}

int download_progress_default_callback(void* clientp, curl_off_t dltotal,
                                       curl_off_t dlnow, curl_off_t ultotal,
                                       curl_off_t ulnow)
{
    return CURL_PROGRESSFUNC_CONTINUE;
}

std::string extract_file_name(const std::string& url)
{
    int i = url.size();
    for (; i >= 0; i--)
    {
        if (url[i] == '/')
        {
            break;
        }
    }

    return url.substr(i + 1, url.size() - 1);
}

bool download(const std::string& url, const std::string& file_path)
{
    const std::string file_name = extract_file_name(url);

    // Hide cursor
    indicators::show_console_cursor(false);

    indicators::ProgressBar progress_bar{
        indicators::option::BarWidth{30}, indicators::option::Start{" ["},
        indicators::option::Fill{"█"}, indicators::option::Lead{"█"},
        indicators::option::Remainder{"-"}, indicators::option::End{"]"},
        indicators::option::PrefixText{file_name},
        // indicators::option::ForegroundColor{indicators::Color::yellow},
        indicators::option::ShowElapsedTime{true},
        indicators::option::ShowRemainingTime{true},
        // indicators::option::FontStyles{
        //     std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
    };

    CURL* curl;
    FILE* fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(file_path.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION,
                         download_progress_callback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA,
                         static_cast<void*>(&progress_bar));
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        // Perform a file transfer synchronously.
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    // Show cursor
    indicators::show_console_cursor(true);

    if (res == CURLE_OK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main(int argc, char* argv[])
{
    std::string url;
    std::string file_path;
    if (argc == 2)
    {
        url = std::string{argv[1]};
        file_path = extract_file_name(url);
    }
    else if (argc == 3)
    {
        url = std::string{argv[1]};
        file_path = std::string{argv[2]};
    }
    else
    {
        throw std::invalid_argument{
            "Insufficient arguments. Please provide URL or "
            "URL and destination file path."};
    }

    download(url, file_path);
}
