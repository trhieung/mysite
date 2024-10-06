// #include <iostream>
// #include <string>
// #include <getopt.h>
// #include <atomic>
// #include <vector>
// #include <fstream>
// #include <iomanip>
// #include <regex>
// #include <mutex>
// #include <memory>
// #include <sstream>
// #include <functional>

// // Global definitions and constants for command indices
// #define CMD_HELP 0
// #define CMD_SHOW 1
// #define CMD_DOWNLOAD 2
// #define CMD_PAUSE 3
// #define CMD_RESUME 4
// #define CMD_DELETE 5
// #define CMD_EXIT 6

// // Struct for storing download task data
// struct CurlData;

// // Class declarations
// class CommandHandler;
// class CurlHandler;

// // Struct to store each download data
// struct CurlData
// {
//     unsigned short id;
//     std::string url;
//     std::string saved_filename;
//     std::string des_folder;
//     std::vector<std::string> status = {"Stop", "Pause", "Downloading"};
//     std::atomic<unsigned long long> curre_down{0};
//     std::atomic<unsigned long long> total_down{100}; // Set a default total download size
//     std::atomic<bool> is_stop{false};
//     std::atomic<bool> is_pause{false};
//     std::atomic<bool> is_complete{false};

//     void print_progress_bar(std::mutex &_syn_print);
//     std::string get_status();
// };

// // Class to handle Curl tasks
// class CurlHandler
// {
// public:
//     CurlHandler();
//     void add_download_task(const unsigned short id, const std::string &url, const std::string &des_folder);
//     void show_all_progress_bars();
//     void pause_task();
//     void resume_task();
//     void delete_task();
//     void stop_task();
//     void stop_all_tasks();

// private:
//     std::vector<std::unique_ptr<CurlData>> curlsData;
//     std::mutex _mutex;
//     bool check_url_format(const std::string &url);
// };

// // Class to handle commands
// class CommandHandler
// {
// public:
//     CommandHandler(CurlHandler *curl_handler);
//     void handler();
//     void execute_command(int command_index);

// private:
//     std::vector<void (CommandHandler::*)()> handler_function;
//     CurlHandler *curl_handler;
//     bool is_running{true};

//     void handler_func0(); // Help
//     void handler_func1(); // Show
//     void handler_func2(std::istringstream &ss); // Download
//     void handler_func3(); // Pause
//     void handler_func4(); // Resume
//     void handler_func5(); // Delete
//     void handler_func6(); // Stop
// };

// // MAIN FUNCTION
// int main(int argc, char *argv[])
// {
//     CurlHandler curl_handler;
//     CommandHandler command_handler(&curl_handler);

//     // Simulate the command handler functionality
//     command_handler.handler();

//     return 0;
// }

// // FUNCTION IMPLEMENTATIONS

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

// CommandHandler::CommandHandler(CurlHandler *curl_handler)
//     : curl_handler(curl_handler)
// {
//     handler_function = {
//         &CommandHandler::handler_func0, // Help
//         &CommandHandler::handler_func1, // Show
//         &CommandHandler::handler_func2, // Download
//         &CommandHandler::handler_func3, // Pause
//         &CommandHandler::handler_func4, // Resume
//         &CommandHandler::handler_func5, // Delete
//         &CommandHandler::handler_func6  // Stop
//     };
// }

// void CommandHandler::handler()
// {
//     std::string command;
//     execute_command(CMD_HELP);
//     std::cout << "Enter command: ";
//     while (is_running)
//     {
//         std::getline(std::cin, command); // Use getline to read the full line

//         std::istringstream ss(command);
//         std::string first_word;
//         ss >> first_word; // Get the first word

//         if (first_word == "0" || first_word == "help" || first_word == "-h" || first_word == "--help")
//             execute_command(CMD_HELP);
//         else if (first_word == "1" || first_word == "show" || first_word == "-S" || first_word == "--show")
//             execute_command(CMD_SHOW);
//         else if (first_word == "2" || first_word == "download" || first_word == "Downloads")
//             handler_func2(ss); // Call handler_func2 with the stream
//         else if (first_word == "3" || first_word == "pause" || first_word == "Pause")
//             execute_command(CMD_PAUSE);
//         else if (first_word == "4" || first_word == "resume" || first_word == "Resume")
//             execute_command(CMD_RESUME);
//         else if (first_word == "5" || first_word == "delete" || first_word == "Delete")
//             execute_command(CMD_DELETE);
//         else if (first_word == "6" || first_word == "exit" || first_word == "Exit")
//             execute_command(CMD_EXIT);
//         else
//             std::cout << "Invalid command. Type 'help' for usage instructions." << std::endl;

//         if (is_running)
//             std::cout << "Enter command: ";
//     }
// }

// void CommandHandler::execute_command(int command_index)
// {
//     (this->*handler_function[command_index])();
// }

// void CommandHandler::handler_func0()
// {
//     std::cout << "Usage:\n"
//               << "\t0. -h, --help\t\t: Display help commands\n"
//               << "\t1. -S, --show\t\t: Show all thread information\n"
//               << "\t2. Downloads [-h] ...\t: Download files\n"
//               << "\t3. Pause <thread id>\t: Pause the specified thread\n"
//               << "\t4. Resume <thread id>\t: Resume the specified thread\n"
//               << "\t5. Delete <thread id>\t: Delete the specified thread\n"
//               << "\t6. Exit: stop program" << std::endl;
// }

// void CommandHandler::handler_func1()
// {
//     // Show all download threads
//     curl_handler->show_all_progress_bars();
// }

// void CommandHandler::handler_func2(std::istringstream &ss)
// {
//     std::string url;
//     std::string des_folder = "."; // Default destination folder

//     // First read URLs until the destination is found or end of input
//     while (ss >> url)
//     {
//         if (url == "-d") // Check if the next argument is a destination folder
//         {
//             if (ss >> des_folder) // Read the destination folder
//                 break; // Exit loop if destination is found
//             else
//             {
//                 std::cout << "Destination folder missing after '-d'." << std::endl;
//                 return; // Exit function if no destination is provided
//             }
//         }
//         unsigned short id = static_cast<unsigned short>(curl_handler->curlsData.size() + 1); // Assign ID based on the current number of tasks
//         curl_handler->add_download_task(id, url, des_folder); // Add download task
//     }
// }

// void CommandHandler::handler_func3()
// {
//     // Simulate pausing a task
//     std::cout << "Pausing download..." << std::endl;
//     curl_handler->pause_task();
// }

// void CommandHandler::handler_func4()
// {
//     // Simulate resuming a task
//     std::cout << "Resuming download..." << std::endl;
//     curl_handler->resume_task();
// }

// void CommandHandler::handler_func5()
// {
//     // Simulate deleting a task
//     std::cout << "Deleting download..." << std::endl;
//     curl_handler->delete_task();
// }

// void CommandHandler::handler_func6()
// {
//     std::string confirmation;
//     std::cout << "Are you sure you want to stop all download tasks? (y/N): ";
//     std::cin >> confirmation;

//     // Check if input is 'y' or 'Y'
//     if (confirmation == "y" || confirmation == "Y")
//     {
//         curl_handler->stop_all_tasks(); // Call to stop all tasks
//         std::cout << "All download tasks have been stopped." << std::endl;
//         this->is_running = false;
//     }
//     else
//     {
//         std::cout << "No changes made. Download tasks continue." << std::endl;
//     }
// }

// CurlHandler::CurlHandler() {}

// void CurlHandler::add_download_task(const unsigned short id, const std::string &url, const std::string &des_folder)
// {
//     if (!check_url_format(url))
//     {
//         std::cout << "Invalid URL format." << std::endl;
//         return;
//     }

//     auto new_data = std::make_unique<CurlData>();
//     new_data->id = id;
//     new_data->url = url;
//     new_data->des_folder = des_folder;
//     curlsData.push_back(std::move(new_data));

//     std::cout << "Download task added: ID=" << id << ", URL=" << url << ", Destination=" << des_folder << std::endl;
// }

// void CurlHandler::show_all_progress_bars()
// {
//     std::cout << "Current Download Tasks:" << std::endl;
//     for (const auto &curl_data : curlsData)
//     {
//         curl_data->print_progress_bar(_mutex);
//     }
// }

// bool CurlHandler::check_url_format(const std::string &url)
// {
//     // Simple regex check for URL format (basic validation)
//     const std::regex url_regex(R"((http|https)://[^\s/$.?#].[^\s]*)");
//     return std::regex_match(url, url_regex);
// }

// void CurlHandler::pause_task()
// {
//     // Implementation to pause tasks
//     std::cout << "Pausing tasks not yet implemented." << std::endl;
// }

// void CurlHandler::resume_task()
// {
//     // Implementation to resume tasks
//     std::cout << "Resuming tasks not yet implemented." << std::endl;
// }

// void CurlHandler::delete_task()
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

int main(){
    return 0;
}