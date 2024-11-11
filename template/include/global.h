// include/global.h

#ifndef GLOBAL_H
#define GLOBAL_H

// Standard Libraries
#include <iostream>

#include <map>
#include <set>
#include <regex>
#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <condition_variable>

#include <getopt.h>
#include <dirent.h> 
#include <unistd.h>

#include <sys/stat.h>
#include <curl/curl.h>
#include <openssl/aes.h>
#include <openssl/err.h>

// Type Aliases
using ull = unsigned long long;
using ll = long long;  // Alias for long long
using ld = long double; // Alias for long double

// Global Constants
const int GLOBAL_CONSTANT = 42; // Example constant
const std::string GLOBAL_STRING = "Hello, World!"; // Example string constant

// Global Variables (extern keyword indicates these are defined elsewhere)
inline int globalVar = 13; // Example global variable declaration

// Other common includes can go here

#endif // GLOBAL_H
