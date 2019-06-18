#include "pch.h"
#include "VkCallParser.h"

TEST(Test_VkCallParser, FeedWithBadFile)
{
	const std::string ApiDumpFile = "vk_apidump1.txt";

	hiveVKT::CVkCallParser Parser;

	ASSERT_EQ(Parser.parse(ApiDumpFile), false);
}

TEST(Test_VkCallParser, VKCallFunctionHeader) 
{
	const std::string ApiDumpFile = "vk_apidump.txt";

	hiveVKT::CVkCallParser Parser;

	ASSERT_EQ(Parser.parse(ApiDumpFile), true);

	const auto& Result = Parser.getVKCallInfoAt(0, 0);
	EXPECT_EQ(Result.size(), 5);

	int SuccessCounter = 0, VoidCounter = 0;
	for (auto Call : Result)
	{
		if (Call.FunctionName == "vkCreateDebugUtilsMessengerEXT")
		{
			EXPECT_TRUE(Call.ReturnValue == "VK_SUCCESS");
			SuccessCounter++;
		}
		if (Call.FunctionName == "vkGetPhysicalDeviceFeatures")
		{
			EXPECT_TRUE(Call.ReturnValue == "void");
			VoidCounter++;
		}
	}
	EXPECT_EQ(SuccessCounter, 1);
	EXPECT_EQ(VoidCounter, 1);
}

TEST(Test_VkCallParser, VKCallFunctionParameters)
{
	const std::string ApiDumpFile = "vk_apidump.txt";

	hiveVKT::CVkCallParser Parser;

	ASSERT_EQ(Parser.parse(ApiDumpFile), true);

	const auto& Result = Parser.getVKCallInfoAt(0, 0);
	EXPECT_EQ(Result.size(), 5);

	EXPECT_EQ(Result[0].ParameterInfo.at("pCreateInfo").first, "const VkInstanceCreateInfo*");
	EXPECT_EQ(Result[0].ParameterInfo.at("pCreateInfo").second, "00000002E03DE9E0");

	EXPECT_EQ(Result[0].ParameterInfo.at("pCreateInfo|pApplicationInfo").first, "const VkApplicationInfo*");
	EXPECT_EQ(Result[0].ParameterInfo.at("pCreateInfo|pApplicationInfo").second, "00000002E03DEF48");

	EXPECT_EQ(Result[0].ParameterInfo.at("pCreateInfo|pApplicationInfo|pEngineName").first, "const char*");
	EXPECT_EQ(Result[0].ParameterInfo.at("pCreateInfo|pApplicationInfo|pEngineName").second, "\"HiveVKT\""); //NOTE: 参数值本身是字符串需要加转义符\

	EXPECT_EQ(Result[0].ParameterInfo.at("pAllocator").first, "const VkAllocationCallbacks*");
	EXPECT_EQ(Result[0].ParameterInfo.at("pAllocator").second, "NULL");
}

TEST(Test_VkCallParser, GetVkCallByFunctionName)
{
	const std::string ApiDumpFile = "vk_apidump.txt";

	hiveVKT::CVkCallParser Parser;

	ASSERT_EQ(Parser.parse(ApiDumpFile), true);

	const auto& VkCallInfoSet = Parser.getVkCallInfoByFunctionName(0, 0, "vkCreateDebugUtilsMessengerEXT");
	EXPECT_EQ(VkCallInfoSet.size(), 1);
	EXPECT_EQ(VkCallInfoSet[0].ReturnValue, "VK_SUCCESS");
}