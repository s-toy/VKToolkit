#include "pch.h"
#include "VkContext.h"

using namespace hiveVKT;

class Test_CreateVkDebugMessenger : public ::testing::Test
{
	virtual void SetUp() override
	{
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());
	}
};

//测试点：DebugUtilsMessenger是否创建成功
TEST_F(Test_CreateVkDebugMessenger, SetupDebugUtilsMessenger_Default)
{
	const CVkDebugUtilsMessenger* m_pMessenger = &(CVkContext::getInstance()->getDebugUtilsMessenger());
	EXPECT_TRUE(m_pMessenger);
//	EXPECT_EQ(m_pMessenger->getWarningAndErrorCount(), 0);
}

//测试点：回调函数是否生效
TEST_F(Test_CreateVkDebugMessenger, DebugUtilsMessengerCallBack)
{

}

//测试点：DebugUtilsMessenger是否销毁成功
TEST_F(Test_CreateVkDebugMessenger, DestroyDebugUtilsMessenger)
{

}