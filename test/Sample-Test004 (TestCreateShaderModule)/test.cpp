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
		m_WarningAndErrorCount = m_pMessenger->getWarningAndErrorCount();
	}

	virtual void TearDown() override
	{
		EXPECT_EQ(m_pMessenger->getWarningAndErrorCount(), m_WarningAndErrorCount);
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	const CVkDebugUtilsMessenger* m_pMessenger = nullptr;
	CVkShaderModuleCreator m_ShaderModuleCreator;
	vk::ShaderModule m_ShaderModule = nullptr;

	uint32_t m_WarningAndErrorCount = 0;
};

//测试点：成功创建默认的ShaderModule
TEST_F(Test_CreateVkShaderModule, CreateShaderModule_Default)
{
	EResult r = m_ShaderModuleCreator.create("VertexShader.spv", m_ShaderModule);
	EXPECT_TRUE(m_ShaderModule);
	EXPECT_EQ(r, EResult::eSuccess);
}

//测试点：测试传入错误文件路径
TEST(Test_ShaderModuleCreator, FeedCreator_WrongFileName)
{
	//文件路径找不到抛出异常
	EXPECT_THROW(m_ShaderModuleCreator.create("WrongPath/VertexShader.spv", m_ShaderModule));
	EXPECT_FALSE(m_ShaderModule);
}

//测试点：测试传入错误格式的文件名
TEST(Test_ShaderModuleCreator, FeedCreator_WrongFormat)
{
	//文件格式错误抛出异常
	EXPECT_THROW(m_ShaderModuleCreator.create("VertexShader.txt", m_ShaderModule));
	EXPECT_FALSE(m_ShaderModule);
}