// #include "CurlHandler.h"

// // FUNCTION IMPLEMENTATIONS

// CurlHandler::CurlHandler() {}

// void CurlHandler::add_download_task(const unsigned short id, const std::string &url, const std::string &des_folder)
// {
//     if (!check_url_format(url))
//     {
//         std::cout << "Invalid URL format." << std::endl;
//         return;
//     }

//     curlsData.emplace_back(std::make_unique<CurlData>(id, url, des_folder));

//     std::cout << "Download task added: ID=" << id << ", URL=" << url << ", Destination=" << des_folder << std::endl;
// }

// void CurlHandler::show_all_tasks()
// {
//     std::cout << "Current Download Tasks:" << std::endl;
//     for (const auto &curl_data : curlsData)
//     {
//         curl_data->print_progress_bar(_mutex);
//     }
// }

// bool CurlHandler::check_url_format(const std::string &url)
// {
//     /*
//     Simple regex check for URL format (basic validation)
//     format: - http://<domain_name>:<port>/path
//             - http://<domain_name>/path
//             - http://<ip>:<port>/path
//             - http://<ip>/path
//     */
//     const std::regex url_regex(R"((http):\/\/([a-zA-Z0-9\-\.]+|[0-9]{1,3}(\.[0-9]{1,3}){3})(:[0-9]{1,5})?(\/[^\s]*)?)");

//     return std::regex_match(url, url_regex);
// }

// void CurlHandler::pause_task(const unsigned short &id)
// {
//     // Implementation to pause tasks
//     std::cout << "Pausing tasks not yet implemented." << std::endl;
// }

// void CurlHandler::resume_task(const unsigned short &id)
// {
//     // Implementation to resume tasks
//     std::cout << "Resuming tasks not yet implemented." << std::endl;
// }

// void CurlHandler::delete_task(const unsigned short &id)
// {
//     // Implementation to delete tasks
//     std::cout << "Deleting tasks not yet implemented." << std::endl;
// }

// void CurlHandler::stop_all_tasks()
// {
//     for (auto &curl_data : curlsData)
//     {
//         curl_data->is_stop = true; // Mark each task as stopped
//     }
//     std::cout << "All tasks have been marked to stop." << std::endl;
// }