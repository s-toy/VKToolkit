#include "pch.h"
#include "VkContext.h"
#include "VkRenderPassCreator.h"
#include "VkDebugMessenger.h"

using namespace hiveVKT;

class Test_CreateVkRenderPass : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		CVkContext::getInstance()->createContext();
		m_Messenger.setupDebugUtilsMessenger();
		m_VkDevice = CVkContext::getInstance()->getVulkanDevice();
	}

	virtual void TearDown() override
	{
		m_Messenger.destroyDebugUtilsMessenger();
		CVkContext::getInstance()->destroyContext();
	}

	CVkRenderPassCreator m_Creator;
	CVkDebugUtilsMessenger m_Messenger;

	vk::Device m_VkDevice = nullptr;
	vk::RenderPass m_VkRenderPass = nullptr;
};

//***********************************************************************************************
//≤‚ ‘µ„:
TEST_F(Test_CreateVkRenderPass, CreateRenderPass_Default)
{
	EXPECT_NO_THROW(m_VkRenderPass = m_Creator.create(m_VkDevice));
	EXPECT_EQ(m_Messenger.getWarningAndErrorCount(), 0);
}

TEST_F(Test_CreateVkRenderPass, CreateRenderPass_OneAttachment)
{
	m_Creator.addAttachment(vk::Format::eR16G16B16A16Sfloat);
	EXPECT_NO_THROW(m_VkRenderPass = m_Creator.create(m_VkDevice));
	EXPECT_EQ(m_Messenger.getWarningAndErrorCount(), 0);
}