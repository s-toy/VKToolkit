#include "pch.h"
#include "VkContext.h"
#include "VkCallParser.h"

using namespace hiveVKT;

class Test_CreateVkDebugMessenger : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		CVkContext::getInstance()->setEnableApiDumpHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	int getDebugUtilsMessengerCallNum()
	{
		hiveVKT::CVkCallParser Parser;
		_ASSERT(Parser.parse("vk_apidump.txt"));
		const auto& ParseResultSet = Parser.getVKCallInfoAt(0, 0);
		int Counter = 0;
		for (auto VkCall : ParseResultSet)
		{
			if (VkCall.FunctionName == "vkCreateDebugUtilsMessengerEXT")
			{
				EXPECT_TRUE(VkCall.ReturnValue == "VK_SUCCESS");
				Counter++;
			}
		}
		return Counter;
	}
};

//测试点：DebugUtilsMessenger创建成功
TEST_F(Test_CreateVkDebugMessenger, SetupDebugUtilsMessenger_Default)
{
	const CVkDebugUtilsMessenger* m_pMessenger = &(CVkContext::getInstance()->getDebugUtilsMessenger());
	EXPECT_TRUE(m_pMessenger);

	EXPECT_EQ(getDebugUtilsMessengerCallNum(), 1);
}

//测试点：回调函数是否生效
TEST_F(Test_CreateVkDebugMessenger, DebugUtilsMessengerCallBack)
{

}

//测试点：DebugUtilsMessenger是否销毁成功
TEST_F(Test_CreateVkDebugMessenger, DestroyDebugUtilsMessenger)
{

}