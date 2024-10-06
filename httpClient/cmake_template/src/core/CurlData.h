#ifndef _CURL_DATA
#define _CURL_DATA

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

// Struct for storing download task data
struct CurlData;

// Struct to store each download data
struct CurlData
{
    unsigned short id;
    std::string url; // url format http://<ip|domain_name>:<port=80>/path_to_filename
    std::string saved_filename;
    std::string des_folder;
    std::vector<std::string> status = {"Stop", "Pause", "Downloading"};
    std::atomic<unsigned long long> curre_down{0};
    std::atomic<unsigned long long> total_down{100}; // Set a default total download size
    std::atomic<bool> is_stop{false};
    std::atomic<bool> is_pause{false};
    std::atomic<bool> is_complete{false};

    CurlData(const unsigned int &_id, const std::string &_url, const std::string &_des_folder);

    void print_progress_bar(std::mutex &_syn_print);
    std::string get_status();
    std::string filename_from_url();
    std::pair<std::string, std::string> get_address();
};

// FUNCTION IMPLEMENTATIONS

CurlData::CurlData(const unsigned int &_id, const std::string &_url, const std::string &_des_folder)
    : id(_id), url(_url), des_folder(_des_folder)
{
    this->saved_filename = filename_from_url();
}

void CurlData::print_progress_bar(std::mutex &_syn_print)
{
    std::lock_guard<std::mutex> lock(_syn_print);
    std::cout << "Thread id: " << this->id << "\t|\t"
              << "Download file from " << url << "\t\t ~\t"
              << this->curre_down << "/" << this->total_down << "\t"
              << std::setprecision(2) << (static_cast<double>(curre_down) / total_down) * 100 << "%\t|\t"
              << this->get_status() << std::endl;
}

std::string CurlData::get_status()
{
    if (this->is_stop)
        return this->status[0];
    else if (this->is_pause)
        return this->status[1];
    return this->status[2];
}

std::string CurlData::filename_from_url()
{
    std::size_t last_slash_pos = url.find_last_of('/'); // Find the last slash position
    if (last_slash_pos != std::string::npos && last_slash_pos + 1 < url.length())
    {
        return url.substr(last_slash_pos + 1); // Extract the filename after the last slash
    }
    return ""; // Return empty if no filename found
}

std::pair<std::string, std::string> CurlData::get_address()
{
    std::pair<std::string, std::string> res{"", ""};
    std::smatch match;
    std::regex url_regex(R"(^http://([^/:]+)(?::(\d+))?)"); // Regex to capture domain/IP and optional port

    // Match the regex pattern against the URL
    if (std::regex_search(url, match, url_regex))
    {
        res.first = match[1];                                  // Capture <ip|domain_name>
        res.second = match[2].matched ? match[2].str() : "80"; // Capture port or default to "80" if no port
    }

    return res;
}

#endif