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

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}
};

//���Ե㣺DebugUtilsMessenger�����ɹ�
TEST_F(Test_CreateVkDebugMessenger, SetupDebugUtilsMessenger_Default)
{
	const CVkDebugUtilsMessenger* m_pMessenger = &(CVkContext::getInstance()->getDebugUtilsMessenger());
	EXPECT_TRUE(m_pMessenger);
	EXPECT_EQ(m_pMessenger->getWarningAndErrorCount(), 0);
}

//���Ե㣺�ص������Ƿ���Ч
TEST_F(Test_CreateVkDebugMessenger, DebugUtilsMessengerCallBack)
{

}

//���Ե㣺DebugUtilsMessenger�Ƿ����ٳɹ�
TEST_F(Test_CreateVkDebugMessenger, DestroyDebugUtilsMessenger)
{

}