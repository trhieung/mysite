#include <gtest/gtest.h>
#include <vector>
#include <string>

int main(int argc, char **argv)
{
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // List of test names to run
    std::vector<std::string> testNames = {
        "ExampleModuleTest.not_void_function_test",
        "ExampleModuleTest.void_function_test"
    };

    // Join the test names with ':' separator
    std::string filter = "";
    for (size_t i = 0; i < testNames.size(); ++i) {
        filter += testNames[i];
        if (i < testNames.size() - 1) {
            filter += ":";
        }
    }

    // Set the filter flag
    ::testing::GTEST_FLAG(filter) = filter;

    // Print the filter to verify
    std::cout << "Running tests: " << filter << std::endl;

    // Run the tests
    return RUN_ALL_TESTS();
}
