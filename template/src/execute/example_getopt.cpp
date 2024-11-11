#include "global.h"
#include "3-parties/custom_getopt.h"

namespace custom_getopt
{
    namespace handle
    {

        bool flag_a(void *data)
        {
            bool print_error = true;
            bool print_log = false;
            std::vector<std::string> *_data = static_cast<std::vector<std::string> *>(data);

            if (_data->size() != 1)
            {
                if (print_error)
                {
                    std::cout << "Error: Expected exactly one argument for the port." << std::endl;
                }
                return false;
            }

            // Define the regex pattern to match "localhost" or an IPv4 address
            std::regex address_pattern(R"(^(localhost|((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9]))$)");

            // Check if _data[0] matches the pattern
            const std::string &address = (*_data)[0];
            if (std::regex_match(address, address_pattern))
            {
                if (print_log)
                {
                    std::cout << "Valid address: " << address << std::endl;
                }
                return true;
            }
            else
            {
                if (print_error)
                {
                    std::cout << "Error: Invalid address format. Expected 'localhost' or an IPv4 address." << std::endl;
                }
                return false;
            }
        }

        bool flag_p(void *data)
        {
            bool print_error = true;
            bool print_log = false;
            std::vector<std::string> *_data = static_cast<std::vector<std::string> *>(data);

            if (_data->size() != 1)
            {
                if (print_error)
                {
                    std::cout << "Error: Expected exactly one argument for the port." << std::endl;
                }
                return false;
            }
            else
            {
                try
                {
                    int port = std::stoi((*_data)[0]); // Access the first element and convert to integer
                    if (print_log)
                    {
                        std::cout << "Handling flag 'p' with port: " << port << std::endl;
                    }
                }
                catch (const std::invalid_argument &)
                {
                    if (print_error)
                    {
                        std::cout << "Error: Port value is not a valid integer." << std::endl;
                    }
                    return false;
                }
                catch (const std::out_of_range &)
                {
                    if (print_error)
                    {
                        std::cout << "Error: Port value is out of range." << std::endl;
                    }
                    return false;
                }
            }
            return true;
        }
    }

    bool run_action_flag(const int &argc, const char **argv,
                         std::vector<std::pair<std::string, std::vector<std::string>>> &inputs,
                         const bool &print_error, const bool &print_logs)
    {
        for (char elem : flag_action_call)
        {
            void *data;
            std::vector<std::string> _values;

            if (elem == 'h') // default implement in custom
            {
                data = (void *)argv[0];
            }
            else if (elem == 'a')
            {
                _values = _options[elem].values; // std::vector<std::string> values
                data = (void *)(&_values);
            }
            else if (elem == 'p')
            {
                _values = _options[elem].values; // std::vector<std::string> values
                data = (void *)(&_values);
            }

            if (!handle::call_function_by_flag(elem, data, func_map))
            {
                if (print_error)
                {
                    std::cout << "error input with flag " << elem << std::endl;
                }
                return false;
            }
        }

        return true;
    }
}

int main(int argc, char **argv)
{
    custom_getopt::add_convention('a', {"address", true, "<address>", "192.168.1.1", "Specify the host IP address", true}); // last flag for action function
    custom_getopt::add_convention('p', {"port", true, "<port>", "8080", "Specify the port number", true});

    // init with action = true like p -> define activate function alre
    custom_getopt::func_map['a'] = custom_getopt::handle::flag_a;
    custom_getopt::func_map['p'] = custom_getopt::handle::flag_p;

    if(!custom_getopt::run(argc, const_cast<const char **>(argv), true, false)){
        return false;
    }

    // handle your action here;
    std::string host = custom_getopt::_options['a'].values[0];
    int port = std::stoi(custom_getopt::_options['p'].values[0]);

    std::cout << "host: " << host << std::endl;
    std::cout << "port: " << port << std::endl;


    return 0;
}
