// #include "CommandHandler.h"

// // FUNCTION IMPLEMENTATIONS
// // class CommandHandler
// CommandHandler::CommandHandler(CurlHandler *curl_handler)
//     : curl_handler(curl_handler),
//       handler_function{
//           &CommandHandler::handler_func0,
//           &CommandHandler::handler_func1,
//           &CommandHandler::handler_func2,
//           &CommandHandler::handler_func3,
//           &CommandHandler::handler_func4,
//           &CommandHandler::handler_func5,
//           &CommandHandler::handler_func6}
// {
// }

// void CommandHandler::handler()
// {
//     std::string commands;
//     execute_command(CMD_HELP, {}); // Execute help command
//     std::cout << "> Enter cmd: ";
//     while (is_running)
//     {
//         std::getline(std::cin, commands); // Use getline to read the full line
//         std::istringstream ss(commands);

//         std::vector<std::string> cmds;
//         std::string cmd;
//         while (ss >> cmd)
//         {
//             cmds.push_back(cmd);
//         }

//         if (cmds[0] == "0" || cmds[0] == "help" || cmds[0] == "-h" || cmds[0] == "--help")
//             execute_command(CMD_HELP, cmds);
//         else if (cmds[0] == "1" || cmds[0] == "show" || cmds[0] == "-S" || cmds[0] == "--show")
//             execute_command(CMD_SHOW, cmds);
//         else if (cmds[0] == "2" || cmds[0] == "download" || cmds[0] == "Downloads")
//             execute_command(CMD_DOWNLOAD, cmds);
//         else if (cmds[0] == "3" || cmds[0] == "pause" || cmds[0] == "Pause")
//             execute_command(CMD_PAUSE, cmds);
//         else if (cmds[0] == "4" || cmds[0] == "resume" || cmds[0] == "Resume")
//             execute_command(CMD_RESUME, cmds);
//         else if (cmds[0] == "5" || cmds[0] == "delete" || cmds[0] == "Delete")
//             execute_command(CMD_DELETE, cmds);
//         else if (cmds[0] == "6" || cmds[0] == "exit" || cmds[0] == "Exit")
//             execute_command(CMD_EXIT, cmds);
//         else
//             std::cout << "Invalid command. Type 'help' for usage instructions." << std::endl;

//         if (is_running)
//             std::cout << "> Enter cmd: ";
//     }
// }

// void CommandHandler::execute_command(int command_index, const std::vector<std::string> &cmds)
// {
//     (this->*handler_function[command_index])(cmds);
// }

// void CommandHandler::print_help_downloads()
// {
//     std::cout << "Usage: Downloads [-h] | [-u <URL1> <URL2> <URL3>] [-d <destination>]\n"
//               << "Options:\n"
//               << "\t-h, --help: Display this help message\n"
//               << "\t-u <URL1> <URL2> <URL3>: URLs of the files to download (up to 3 URLs)\n"
//               << "\t-d <destination>: Destination folder (default is current directory)\n"
//               << "Examples:\n"
//               << "\t Downloads -h\n"
//               << "\t Downloads -u http://example.com/file1.zip http://example.com:8080/file2.zip\n"
//               << "\t Downloads -u http://localhost/image1.png http://127.0.0.1:8080/image2.png -d /path/to/destination\n";
// }

// void CommandHandler::handler_func0(const std::vector<std::string> &cmds)
// {
//     std::cout << "Usage:\n"
//               << "\t0. -h, --help\t\t: Display help commands\n"
//               << "\t1. -S, --show\t\t: Show all thread information\n"
//               << "\t2. Downloads [-h] ...\t: Download files\n"
//               << "\t3. Pause <thread id>\t: Pause the specified thread\n"
//               << "\t4. Resume <thread id>\t: Resume the specified thread\n"
//               << "\t5. Delete <thread id>\t: Delete the specified thread\n"
//               << "\t6. Exit\t\t\t: Stop program" << std::endl;
// }

// void CommandHandler::handler_func1(const std::vector<std::string> &cmds)
// {
//     // Show all download threads
//     curl_handler->show_all_tasks();
// }

// void CommandHandler::handler_func2(const std::vector<std::string> &cmds)
// {
//     // Handle help command
//     if (cmds.size() == 1 || (cmds.size() == 2 && (cmds[1] == "-h" || cmds[1] == "--help")))
//     {
//         print_help_downloads();
//         return; // Exit after displaying help
//     }

//     std::vector<std::string> temp_urls; // Temporary vector for URLs
//     std::string des_folder = ".";       // Default destination folder

//     // Read URLs until the destination is found or end of input
//     for (size_t i = 1; i < cmds.size(); ++i) // Start from 1 to skip the command itself
//     {
//         if (cmds[i] == "-d") // Check if the next argument is a destination folder
//         {
//             if (i + 1 < cmds.size()) // Read the destination folder
//             {
//                 des_folder = cmds[++i]; // Move to the destination folder
//                 // Create the destination folder (implement the function)
//                 // For example: create_folder(des_folder);
//                 std::cout << "Creating folder: " << des_folder << std::endl;
//             }
//             else
//             {
//                 std::cout << "Error: Destination folder missing after '-d'." << std::endl;
//                 return; // Exit function if no destination is provided
//             }
//             continue; // Continue to the next command
//         }
//         else
//         {
//             temp_urls.push_back(cmds[i]); // Add URL to the temporary list
//         }
//     }

//     // Check if any URLs were provided
//     if (temp_urls.empty())
//     {
//         std::cout << "No URLs provided for download. Default destination: " << des_folder << std::endl;
//         return; // Exit if no URLs are provided
//     }

//     // Add download tasks for each URL
//     for (size_t i = 0; i < temp_urls.size(); ++i)
//     {
//         if (curl_handler->check_url_format(temp_urls[i]))
//         {
//             curl_handler->add_download_task(static_cast<unsigned short>(i + 1), temp_urls[i], des_folder);
//         }
//         else
//         {
//             std::cout << "Invalid URL: " << temp_urls[i] << std::endl; // Print invalid URLs
//         }
//     }
// }

// void CommandHandler::handler_func3(const std::vector<std::string> &cmds)
// {
//     // Handle pause functionality
//     if (cmds.size() < 2) // Check if thread ID is provided
//     {
//         std::cout << "Error: No thread ID provided to pause." << std::endl;
//         return;
//     }

//     unsigned short thread_id = static_cast<unsigned short>(std::stoi(cmds[1])); // Convert thread ID from string to unsigned short
//     curl_handler->pause_task(thread_id);                                        // Implement the pause task using thread_id
// }

// void CommandHandler::handler_func4(const std::vector<std::string> &cmds)
// {
//     // Handle resume functionality
//     if (cmds.size() < 2) // Check if thread ID is provided
//     {
//         std::cout << "Error: No thread ID provided to resume." << std::endl;
//         return;
//     }

//     unsigned short thread_id = static_cast<unsigned short>(std::stoi(cmds[1])); // Convert thread ID from string to unsigned short
//     curl_handler->resume_task(thread_id);                                       // Implement the resume task using thread_id
// }

// void CommandHandler::handler_func5(const std::vector<std::string> &cmds)
// {
//     // Handle delete functionality
//     if (cmds.size() < 2) // Check if thread ID is provided
//     {
//         std::cout << "Error: No thread ID provided to delete." << std::endl;
//         return;
//     }

//     unsigned short thread_id = static_cast<unsigned short>(std::stoi(cmds[1])); // Convert thread ID from string to unsigned short
//     curl_handler->delete_task(thread_id);                                       // Implement the delete task using thread_id
// }

// void CommandHandler::handler_func6(const std::vector<std::string> &cmds)
// {
//     // Handle exit functionality
//     std::string confirmation;

//     std::cout << "Are you sure you want to stop all download tasks? (y/N): ";

//     // Use std::getline to read the entire line, allowing for spaces and empty input
//     std::getline(std::cin, confirmation);

//     // Trim leading and trailing whitespace from the input (optional)
//     confirmation.erase(confirmation.find_last_not_of(" \n\r\t") + 1);
//     confirmation.erase(0, confirmation.find_first_not_of(" \n\r\t"));

//     // Check if input is 'y' or 'Y', or handle empty input
//     if (confirmation.empty())
//     {
//         std::cout << "No input received. Download tasks continue." << std::endl;
//     }
//     else if (confirmation == "y" || confirmation == "Y")
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