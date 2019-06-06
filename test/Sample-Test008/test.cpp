#include "pch.h"
#include "../../vkt/VkUtility.h"

TEST(Test_VkUtility, TestSplitByStr) {
	std::string Input = "vkCreateInstance(pCreateInfo, pAllocator, pInstance) returns VkResult VK_SUCCESS (0):";
	std::vector<std::string> Output;

	EXPECT_TRUE(hiveVKT::splitByStr(Input, "returns", Output));
	EXPECT_EQ("vkCreateInstance(pCreateInfo, pAllocator, pInstance) ", Output[0]);
	EXPECT_EQ(" VkResult VK_SUCCESS (0):", Output[1]);
}