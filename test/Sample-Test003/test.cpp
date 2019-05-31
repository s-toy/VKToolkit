#include "pch.h"
#include "VkContext.h"
#include "VkRenderPassCreator.h"
#include "VkSwapchain.h"

using namespace hiveVKT;

class Test_CreateVkRenderPass : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());
		m_VkDevice = CVkContext::getInstance()->getVulkanDevice();
		ASSERT_TRUE(m_VkDevice);
		m_pMessenger = &(CVkContext::getInstance()->getDebugUtilsMessenger());
		ASSERT_TRUE(m_pMessenger);
		//EXPECT_EQ(m_pMessenger->getWarningAndErrorCount(), 0);
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
		//EXPECT_EQ(m_pMessenger->getWarningAndErrorCount(), 0);
	}

	CVkRenderPassCreator m_Creator;
	const CVkDebugUtilsMessenger* m_pMessenger = nullptr;

	vk::Device m_VkDevice = nullptr;
	vk::RenderPass m_VkRenderPass = nullptr;
};

//***********************************************************************************************
//≤‚ ‘µ„: 
TEST_F(Test_CreateVkRenderPass, CreateRenderPass_Default)
{
	vk::Result r = m_Creator.create(m_VkDevice, m_VkRenderPass);
}

TEST_F(Test_CreateVkRenderPass, CreateRenderPass_OneAttachment)
{

}