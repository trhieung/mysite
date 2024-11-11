#ifndef _CUSTOM_GETOPT_H_
#define _CUSTOM_GETOPT_H_

#include "global.h"

namespace custom_getopt
{
    // Struct to store information about command-line options
    struct struct_opt
    {
        std::string long_flag; // Long flag representation (e.g., "help")

        // Show variable
        bool required;                    // Whether the option is required
        std::string argument_placeholder; // Placeholder for the argument (e.g., "<host_ip>")
        std::string argument_example;     // Example of a valid argument (e.g., "192.168.1.1")
        std::string description;          // Description of what the flag does (e.g., "Show help message")

        // Value variable
        bool action;
        bool value_requeires;
        std::vector<std::string> values = {}; // Store values associated with the flag

        // Constructor to initialize the struct
        struct_opt(std::string long_flag = "",
                   bool required = false,
                   std::string argument_placeholder = "",
                   std::string argument_example = "",
                   std::string description = "",
                   bool action = false,
                   bool value_requeires = false,
                   std::vector<std::string> values = {})
            : long_flag(long_flag),
              required(required),
              argument_placeholder(argument_placeholder),
              argument_example(argument_example),
              description(description),
              action(action),
              value_requeires(value_requeires),
              values(values) {}
    };

    extern std::string desc;
    extern std::unordered_map<char, custom_getopt::struct_opt> _options;
    extern std::unordered_map<std::string, char> flag_long_to_short;
    extern std::unordered_map<char, std::string> flag_short_to_long;
    extern std::set<char> flag_action;
    extern std::set<char> flag_action_call;
    extern std::unordered_map<char, std::function<bool(void *)>> func_map;
    std::set<char> flag_required;

    // Core function
    namespace handle
    {
        // Function to look up and call the function by name
        bool call_function_by_flag(const char &func_name, void *args, const std::unordered_map<char, std::function<bool(void *)>> &func_map);

        // function handle for each flag
        bool flag_h(void *data);

    }

    bool run(const int &argc, const char **argv,
             const bool &print_error = true, const bool &print_logs = false);

    void add_convention(const char &short_flag, const struct_opt &value);

    // Test functions
    void test_main(const std::string &input = "./custom_getopt -h huhu --help ./asdf huhu -a address -p asdf");

    // Support functions
    bool run_action_flag(const int &argc, const char **argv,
                         std::vector<std::pair<std::string, std::vector<std::string>>> &inputs,
                         const bool &print_error = true, const bool &print_logs = false);
    void inputFormat(const int &argc, const char **argv, std::vector<std::pair<std::string, std::vector<std::string>>> &inputs);
    void stringToArgv(const std::string &input, int &argc, char **&argv);
    bool flag_check(const std::string &flag, const bool &print_error = true);
    bool flag_short_check(const char &flag_short, const bool &print_error = true);
    bool flag_long_check(const std::string &flag_long, const bool &print_error = true);
};

// Init
inline std::string custom_getopt::desc = "";
inline std::unordered_map<char, custom_getopt::struct_opt> custom_getopt::_options = {{'h', {"help", false, "", "", "Show help message", true, false, {}}}};

inline std::unordered_map<std::string, char> custom_getopt::flag_long_to_short = {{"help", 'h'}};
inline std::unordered_map<char, std::string> custom_getopt::flag_short_to_long = {{'h', "help"}};
inline std::set<char> custom_getopt::flag_action = {};
inline std::set<char> custom_getopt::flag_action_call = {};
inline std::unordered_map<char, std::function<bool(void *)>> custom_getopt::func_map = {};
// inline std::set<char> custom_getopt::flag_action = {'h'};
// inline std::set<char> custom_getopt::flag_action_call = {'h'};
// inline std::unordered_map<char, std::function<bool(void *)>> custom_getopt::func_map = {{'h', custom_getopt::handle::flag_h}};

// Core functions
inline bool custom_getopt::handle::call_function_by_flag(const char &func_name, void *args, const std::unordered_map<char, std::function<bool(void *)>> &func_map)
{
    auto it = func_map.find(func_name);
    if (it != func_map.end())
    {
        // Call the function with args
        return it->second(args);
    }
    else
    {
        std::cout << "Function " << func_name << " not found!\n";
        return false;
    }
}

inline bool custom_getopt::handle::flag_h(void *data)
{
    const char *program_name = static_cast<const char *>(data);

    std::cout << "Usage: " << program_name << " [options]\nOptions:\n";

    std::cout << std::left << std::setw(20) << "Flag"
              << std::setw(15) << "Argument"
              << std::setw(15) << "Example"
              << std::setw(10) << "Required"
              << "Description\n";
    std::cout << std::string(80, '-') << '\n';

    for (const auto &item : _options)
    {
        const auto &opt = item.second;
        std::cout << std::left << std::setw(20) << ("--" + opt.long_flag)
                  << std::setw(15) << opt.argument_placeholder
                  << std::setw(15) << opt.argument_example
                  << std::setw(10) << (opt.required ? "Yes" : "No")
                  << opt.description << '\n';
    }
    return true;
}

inline bool custom_getopt::run(const int &argc, const char **argv,
                               const bool &print_error, const bool &print_logs)
{
    if (argc == 1 || (argc == 2 && (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)))
    {
        handle::flag_h((void *)argv[0]);
        return false;
    }

    std::vector<std::pair<std::string, std::vector<std::string>>> inputs;
    inputFormat(argc, argv, inputs);

    // check flag
    for (const auto &input : inputs)
    {

        char input_short;
        if (input.first.length() > 2 && input.first.substr(0, 2) == "--")
            input_short = flag_long_to_short[input.first.substr(2)];
        else if (input.first.length() == 2 && input.first[0] == '-')
            input_short = input.first[1];
        else
        {
            continue;
        }

        if (!flag_short_check(input_short, false))
        {
            std::cout << "Invalid flag: " << input.first << std::endl;
            return false;
        }

        // update value
        _options[input_short].values = input.second;

        if (print_logs)
        {
            std::cout << "Flag: " << input.first << "\t Values: ";
            for (const auto &val : input.second)
            {
                std::cout << val << ", ";
            }
            std::cout << std::endl;
        }
    }

    // Check required flags in the inputs
    for (const auto &required_flag : flag_required)
    {
        bool found_required = false;
        for (const auto &input : inputs)
        {
            char input_short;
            if (input.first.length() > 2 && input.first.substr(0, 2) == "--")
                input_short = flag_long_to_short[input.first.substr(2)];
            else if (input.first.length() == 2 && input.first[0] == '-')
                input_short = input.first[1];
            else
                continue;

            if (required_flag == input_short)
            {
                found_required = true;
                break;
            }
        }

        if (!found_required)
        {
            if (print_error)
            {
                std::cerr << "Missing required argument: " << flag_short_to_long[required_flag] << " (" << required_flag << ")" << std::endl;
            }
            return false;
        }
    }

    // for flag in in inputs -> handle
    for (const auto &action_flag : flag_action)
    {
        for (const auto &input : inputs)
        {
            char input_short;
            if (input.first.length() > 2 && input.first.substr(0, 2) == "--")
                input_short = flag_long_to_short[input.first.substr(2)];
            else if (input.first.length() == 2 && input.first[0] == '-')
                input_short = input.first[1];
            else
                continue;

            if (action_flag == input_short)
            {
                flag_action_call.insert(input_short);
            }
        }
    }

    return run_action_flag(argc, argv, inputs, print_error, print_logs);

    // return true; // Return success if all flags are valid
}

// inline void custom_getopt::run_action_flag(const int &argc, const char **argv,
//                                               std::vector<std::pair<std::string, std::vector<std::string>>> &inputs,
//                                               const bool &print_error, const bool &print_logs) {
//     for (char elem : flag_action_call)
//     {
//         void *data;
//         if (elem == 'h')
//         {
//             data = (void *)argv[0];
//         }

//         handle::call_function_by_flag(elem, data, func_map);
//     }
// }

inline void custom_getopt::add_convention(const char &short_flag, const custom_getopt::struct_opt &value)
{
    // Add the new option to the options map using the short flag as the key
    _options[short_flag] = value;

    // Update the mappings for long to short and short to long flags
    flag_long_to_short[value.long_flag] = short_flag;
    flag_short_to_long[short_flag] = value.long_flag;

    // If the option is required, add it to the required flags set
    if (value.required)
    {
        flag_required.insert(short_flag);
    }

    // If the option defines an action, add it to the action flags set
    if (value.action)
    {
        flag_action.insert(short_flag);
        flag_action_call.insert(short_flag);
        // char short_flag = 'h';
        // inline std::unordered_map<char, std::function<void(void *)>> custom_getopt::func_map = {{short_flag, custom_getopt::handle::f"flag_{short_flag}"}};
    }
}

// Test functions
inline void custom_getopt::test_main(const std::string &input)
{
    int argc;
    char **argv;

    /*
    Correct the argument order to match the struct_opt constructor
    if you add action flag = true -> go to init command in this file then check the add function by flag and handle it
    */
    custom_getopt::add_convention('a', {"address", true, "<address>", "192.168.1.1", "Specify the host IP address"});
    custom_getopt::add_convention('p', {"port", true, "<port>", "8080", "Specify the port number"});

    custom_getopt::stringToArgv(input, argc, argv);
    // print(argv[0]);

    custom_getopt::run(argc, const_cast<const char **>(argv), true, false);

    delete[] argv; // Proper cleanup for dynamically allocated memory in test because the dynamic create in stringToArgv
}

// Support function
inline void custom_getopt::stringToArgv(const std::string &input, int &argc, char **&argv)
{
    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> tokens;

    // Split the input string by spaces
    while (std::getline(ss, token, ' '))
    {
        tokens.push_back(token);
    }

    argc = tokens.size();
    argv = new char *[argc];

    for (int i = 0; i < argc; ++i)
    {
        argv[i] = new char[tokens[i].length() + 1];
        std::strcpy(argv[i], tokens[i].c_str());
    }
}

inline void custom_getopt::inputFormat(const int &argc, const char **argv, std::vector<std::pair<std::string, std::vector<std::string>>> &inputs)
{
    int i = 1; // Start from 1 to skip the program name in argv[0]

    while (i < argc)
    {
        std::pair<std::string, std::vector<std::string>> input;

        // Check if the current argument starts with '-'
        if (argv[i][0] != '-')
        {
            ++i;
            continue; // Skip non-flag arguments
        }

        input.first = argv[i++]; // Store the flag (e.g., "-h")

        // Collect values associated with this flag until we hit another flag
        while (i < argc && argv[i][0] != '-')
        {
            input.second.push_back(argv[i++]); // Collect values associated with the flag
        }

        // Add the collected flag and its values to the inputs vector
        if (!input.first.empty())
        {
            inputs.push_back(input);
        }
    }
}

inline bool custom_getopt::flag_check(const std::string &flag, const bool &print_error)
{
    if (flag.length() == 2 && flag[0] == '-')
    {
        return flag_short_check(flag[1], print_error);
    }
    if (flag.length() >= 3 && flag[0] == '-' && flag[1] == '-')
    {
        std::string _flag = flag.substr(2);
        return flag_long_check(_flag, print_error);
    }
    return false;
}

inline bool custom_getopt::flag_short_check(const char &flag_short, const bool &print_error)
{
    auto it = flag_short_to_long.find(flag_short);
    if (it != flag_short_to_long.end())
    {
        return true;
    }
    else
    {
        if (print_error)
        {
            std::cout << "Invalid flag: " << flag_short << std::endl;
        }
        return false;
    }
}

inline bool custom_getopt::flag_long_check(const std::string &flag_long, const bool &print_error)
{
    auto it = flag_long_to_short.find(flag_long);
    if (it != flag_long_to_short.end())
    {
        return true;
    }
    else
    {
        if (print_error)
        {
            std::cout << "Invalid flag: " << flag_long << std::endl;
        }
        return false;
    }
}

#endif // _CUSTOM_GETOPT_H_v1_
