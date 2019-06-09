#include "pch.h"
#include "VkCallParser.h"

TEST(Test_VkCallParser, FeedWithBadFile)
{
	const std::string ApiDumpFile = "vk_apidump1.txt";

	hiveVKT::CVkCallParser Parser;

	ASSERT_EQ(Parser.parse(ApiDumpFile), false);
}

TEST(Test_VkCallParser, NormalPattern) 
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