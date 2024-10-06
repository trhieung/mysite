// #include "CurlData.h"

// // FUNCTION IMPLEMENTATIONS
// // Struct to store each download data

// CurlData::CurlData(const unsigned int &_id, const std::string &_url, const std::string &_des_folder)
//     : id(_id), url(_url), des_folder(_des_folder)
// {
//     this->saved_filename = filename_from_url();
// }

// void CurlData::print_progress_bar(std::mutex &_syn_print)
// {
//     std::lock_guard<std::mutex> lock(_syn_print);
//     std::cout << "Thread id: " << this->id << "\t|\t"
//               << "Download file from " << url << "\t\t ~\t"
//               << this->curre_down << "/" << this->total_down << "\t"
//               << std::setprecision(2) << (static_cast<double>(curre_down) / total_down) * 100 << "%\t|\t"
//               << this->get_status() << std::endl;
// }

// std::string CurlData::get_status()
// {
//     if (this->is_stop)
//         return this->status[0];
//     else if (this->is_pause)
//         return this->status[1];
//     return this->status[2];
// }

// std::string CurlData::filename_from_url()
// {
//     std::size_t last_slash_pos = url.find_last_of('/'); // Find the last slash position
//     if (last_slash_pos != std::string::npos && last_slash_pos + 1 < url.length())
//     {
//         return url.substr(last_slash_pos + 1); // Extract the filename after the last slash
//     }
//     return ""; // Return empty if no filename found
// }
