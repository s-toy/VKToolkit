#include "pch.h"
#include "VkContext.h"
#include "VkShaderModuleCreator.h"
#include "VkCallParser.h"

using namespace hiveVKT;

class Test_CreateVkShaderModule : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		CVkContext::getInstance()->enableContextFeature(ENABLE_DEBUG_UTILS | ENABLE_API_DUMP);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());

		m_WarningAndErrorCount = CVkContext::getInstance()->getWarningAndErrorCount();
	}

	virtual void TearDown() override
	{
		EXPECT_EQ(CVkContext::getInstance()->getWarningAndErrorCount(), m_WarningAndErrorCount);
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	int getCreateShaderModuleCallNum()
	{
		hiveVKT::CVkCallParser Parser;
		_ASSERT(Parser.parse("vk_apidump.txt"));
		auto CreateShaderModuleCallInfo = Parser.getVkCallInfoByFunctionName(0, 0, "vkCreateShaderModule");

		return CreateShaderModuleCallInfo.size();
	}

	CVkShaderModuleCreator m_ShaderModuleCreator;
	vk::ShaderModule m_ShaderModule = nullptr;

	uint32_t m_WarningAndErrorCount = 0;
};

//测试点：成功创建默认的ShaderModule
TEST_F(Test_CreateVkShaderModule, CreateShaderModule_Default)
{
	m_ShaderModule = m_ShaderModuleCreator.create("VertexShader.spv");
	EXPECT_TRUE(m_ShaderModule);
	
	EXPECT_EQ(getCreateShaderModuleCallNum(), 1);
}

//测试点：测试传入错误文件路径
TEST_F(Test_CreateVkShaderModule, FeedCreator_WrongFileName)
{
	m_ShaderModule = m_ShaderModuleCreator.create("WrongPath/VertexShader.spv");
	EXPECT_FALSE(m_ShaderModule);

	EXPECT_EQ(getCreateShaderModuleCallNum(), 0);
}

//测试点：测试传入错误格式的文件名
TEST_F(Test_CreateVkShaderModule, FeedCreator_WrongFormat)
{
	m_ShaderModule = m_ShaderModuleCreator.create("VertexShader.txt");
	EXPECT_FALSE(m_ShaderModule);

	EXPECT_EQ(getCreateShaderModuleCallNum(), 0);
}