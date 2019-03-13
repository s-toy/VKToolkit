#include "pch.h"
#include "Utility.hpp"

using namespace hiveVKT;

//*******************************************************************************
//TEST POINT: To verify that the hiveVKT::format function can return an expected string.
TEST(Test_StringFormatting, Format)
{
	EXPECT_EQ(format(nullptr), std::string{});
	EXPECT_EQ(format(""), "");
	EXPECT_EQ(format(" "), " ");
	EXPECT_EQ(format("chaos"), "chaos");
	EXPECT_EQ(format("str%s%", "ing"), "string");
	EXPECT_EQ(format("%s %i!", "hello", 9102), "hello 9102!");
	EXPECT_EQ(format("%.2f", 3.1415926), "3.14");
}