#include "pch.h"
#include "VkContext.h"
#include "VkShaderModuleCreator.h"

using namespace hiveVKT;

class Test_CreateVkShaderModule : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());

		m_pMessenger = &(CVkContext::getInstance()->getDebugUtilsMessenger());
		ASSERT_TRUE(m_pMessenger);
	//	EXPECT_EQ(m_pMessenger->getWarningAndErrorCount(), 0);
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	//	EXPECT_EQ(m_pMessenger->getWarningAndErrorCount(), 0);
	}

	const CVkDebugUtilsMessenger* m_pMessenger = nullptr;
	CVkShaderModuleCreator m_ShaderModuleCreator;
};

//���Ե㣺������ȷ�ļ�������ShaderModule
TEST_F(Test_CreateVkShaderModule, CreateShaderModule_Default)
{

}

//���Ե㣺���Դ�������ļ���
TEST(Test_ShaderModuleCreator, FeedCreator_WrongFileName)
{

}

//���Ե㣺���Դ�������ʽ�ļ���
TEST(Test_ShaderModuleCreator, FeedCreator_WrongFormat)
{

}
