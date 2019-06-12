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

//���Ե㣺DebugUtilsMessenger�����ɹ�
TEST_F(Test_CreateVkDebugMessenger, SetupDebugUtilsMessenger_Default)
{
	const CVkDebugUtilsMessenger* m_pMessenger = &(CVkContext::getInstance()->getDebugUtilsMessenger());
	EXPECT_TRUE(m_pMessenger);

	EXPECT_EQ(getDebugUtilsMessengerCallNum(), 1);
}

//���Ե㣺�ص������Ƿ���Ч
TEST_F(Test_CreateVkDebugMessenger, DebugUtilsMessengerCallBack)
{

}

//���Ե㣺DebugUtilsMessenger�Ƿ����ٳɹ�
TEST_F(Test_CreateVkDebugMessenger, DestroyDebugUtilsMessenger)
{

}