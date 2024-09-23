#include <gtest/gtest.h>
#include "header2.h"

TEST(Header2Test, NotionHeader)
{
    Header2 h;
    testing::internal::CaptureStdout(); // Capture stdout
    h.notionHeader();
    std::string output = testing::internal::GetCapturedStdout(); // Get captured output
    EXPECT_EQ(output, "Hi from header 2\n");                     // Check the output
}
