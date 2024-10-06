// test1.cpp
#include <gtest/gtest.h>
#include "header1.h"

TEST(Header1Test, NotionHeader)
{
    Header1 h;
    testing::internal::CaptureStdout(); // Capture stdout
    h.notionHeader();
    std::string output = testing::internal::GetCapturedStdout(); // Get captured output
    EXPECT_EQ(output, "Hi from header 1\n");                     // Check the output
}
