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
//���Ե�: ����Ĭ�ϵ�RenderPass
TEST_F(Test_CreateVkRenderPass, CreateDefaultRenderPass)
{
	vk::Result r = m_Creator.create(m_VkDevice, m_VkRenderPass);
	EXPECT_FALSE(m_VkRenderPass);			//����RenderPassҪ��������һ��Subpass������Ӧ�÷��ؿյ�Handle
	EXPECT_EQ(r, vk::Result::eIncomplete);
}

//***********************************************************************************************
//���Ե�: ������һ��Subpass��RenderPass
TEST_F(Test_CreateVkRenderPass, CreateWithOneSubpass)
{
	vk::AttachmentDescription ColorAttachment;
	ColorAttachment.format = vk::Format::eR16G16B16A16Sfloat;
	ColorAttachment.samples = vk::SampleCountFlagBits::e1;
	ColorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	ColorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	ColorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	ColorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	ColorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	ColorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
	m_Creator.addAttachment(vk::Format::eR16G16B16A16Sfloat);

	SSubPassDescription SubpassDesc;
	vk::AttachmentReference ColorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);
	SubpassDesc.ColorAttachmentSet = { ColorAttachmentRef };

	m_Creator.addSubpass(SubpassDesc);
	vk::Result r = m_Creator.create(m_VkDevice, m_VkRenderPass);
}

//***********************************************************************************************
//���Ե�: Subpass���Ϸ������
TEST_F(Test_CreateVkRenderPass, CreateWithInvalidSubpass)
{
	SSubPassDescription SubpassDesc;
	m_Creator.addSubpass(SubpassDesc);
	vk::Result r = m_Creator.create(m_VkDevice, m_VkRenderPass);
	EXPECT_FALSE(m_VkRenderPass);
	EXPECT_EQ(r, vk::Result::eIncomplete);
}

//***********************************************************************************************
//���Ե㣺����ʱ������Ч��Deivce
TEST_F(Test_CreateVkRenderPass, CreateWithInvalidDevice)
{
		vk::Device Device;
		vk::Result r = m_Creator.create(Device, m_VkRenderPass);	//1. Deviceδ����
	
		r = m_Creator.create(nullptr, m_VkRenderPass);	//2. �����ָ��
}

//***********************************************************************************************
//���Ե�: ����������Subpass��RenderPass
TEST_F(Test_CreateVkRenderPass, CreateWithTwoSubpass)
{
	//SSubPassDescription SubpassDesc1;
	//m_Creator.addSubpass(SubpassDesc1);
	//SSubPassDescription SubpassDesc2;
	//m_Creator.addSubpass(SubpassDesc2);
	//vk::Result r = m_Creator.create(m_VkDevice, m_VkRenderPass);
}

//***********************************************************************************************
//���Ե㣺
TEST_F(Test_CreateVkRenderPass, AddAttachment)
{

}

//***********************************************************************************************
//���Ե㣺
TEST_F(Test_CreateVkRenderPass, InvalidAttachment)
{

}

//***********************************************************************************************
//���Ե㣺
TEST_F(Test_CreateVkRenderPass, AddSubpassDependency)
{

}