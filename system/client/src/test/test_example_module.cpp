#include <gtest/gtest.h>
#include "3-parties/example_module.h"

// Test case for non-void function
TEST(ExampleModuleTest, not_void_function_test)
{
    // Test case 1: Call the function and capture the result
    bool result = example_module::test_valid_string();  // Add missing semicolon
    EXPECT_EQ(result, true);  // Expect the result to be false
}

// Test case for void function that captures output
TEST(ExampleModuleTest, void_function_test)
{
    // Capture the standard output
    testing::internal::CaptureStdout();

    // Call the function that prints to stdout
    example_module::notion();

    // Get the captured output
    std::string output = testing::internal::GetCapturedStdout();

    // Define the expected output
    std::string expected_output = R"(
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

    // Compare the captured output with the expected output
    EXPECT_EQ(output, expected_output);
}
