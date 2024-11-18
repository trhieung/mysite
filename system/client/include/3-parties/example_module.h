#ifndef _EXAMPLE_MODULE_H_
#define _EXAMPLE_MODULE_H_

#include "global.h"
namespace example_module
{
    extern std::string desc;

    void notion();
    bool test_valid_string();
};

inline std::string example_module::desc = R"(
------------------------------ Notion from Example Module -------------------------------
|(+) Purpose:                                                                           |
|    (-) This module provides functionality for [insert brief purpose here].            |
|                                                                                       |
|(+) Support Functions:                                                                 |
|                                                                                       |
|    |---------------------------------------------------------------------------|      |
|    | Function Name | Input (argv0: type, argv1: type)  | Output (return type)  |      |
|    |---------------|-----------------------------------|-----------------------|      |
|    | notion        | none                              | none                  |      |
|                                                                                       |
-----------------------------------------------------------------------------------------

)";

inline void example_module::notion()
{
    std::cout << desc;
}

inline bool example_module::test_valid_string()
{
    return desc == "" ? false : true;
}
#endif
