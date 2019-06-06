#include "pch.h"
#include "VkCallParser.h"

void ValidateVkCreateInstance(const hiveVKT::TParseResult& vResult)
{
	auto VkCallSet = vResult.find(std::make_pair(0, 0));

	int Counter = 0;
	for (auto Call : VkCallSet)
	{
		if (Call.FunctionName == "vkCreateInstance")
		{
			EXPECT_TRUE(Call.RetureValue == "VK_SUCCESS");
			Counter++;
		}
	}
	EXPECT_EQ(Counter, 1);
}

void ValidateNonInvokedVkCall(const hiveVKT::TParseResult& vResult)
{
	int Counter = 0;
	for (auto Value : vResult)
	{
		for (auto Call : Value)
			if (Call.FunctionName == "vkCreateGraphicsPipelines")
				Counter++;
	}

	EXPECT_EQ(Counter, 0);
}

TEST(Test_VkCallParser, FeedWithBadFile)
{
	const std::string ApiDumpFile = "vk_apidump1.txt";

	hiveVKT::CVkCallParser Parser;

	auto ParseResult = Parser.parse(ApiDumpFile);

	EXPECT_EQ(ParseResult.size(), 0);
}

TEST(Test_VkCallParser, NormalPattern) 
{
	const std::string ApiDumpFile = "vk_apidump.txt";

	hiveVKT::CVkCallParser Parser;

	auto ParseResult = Parser.parse(ApiDumpFile);

	EXPECT_EQ(vResult.size(), 1);

	ValidateVkCreateInstance(ParseResult);
	ValidateNonInvokedVkCall(ParseResult);
}