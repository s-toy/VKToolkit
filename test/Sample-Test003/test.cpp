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

		m_WarningAndErrorCount = CVkContext::getInstance()->getWarningAndErrorCount();
	}

	virtual void TearDown() override
	{
		EXPECT_EQ(CVkContext::getInstance()->getWarningAndErrorCount(), m_WarningAndErrorCount);

		if (m_VkRenderPass) m_VkDevice.destroyRenderPass(m_VkRenderPass);
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	CVkRenderPassCreator m_Creator;

	vk::Device m_VkDevice = nullptr;
	vk::RenderPass m_VkRenderPass = nullptr;

	uint32_t m_WarningAndErrorCount = 0;
};

//***********************************************************************************************
//���Ե�: ����Ĭ�ϵ�RenderPass
TEST_F(Test_CreateVkRenderPass, CreateDefaultRenderPass)
{
	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_FALSE(m_VkRenderPass);			//����RenderPassҪ��������һ��Subpass������Ӧ�÷��ؿյ�Handle
	EXPECT_EQ(r, EResult::eErrorInvalidParameters);
}

//***********************************************************************************************
//���Ե�: �ɹ�������һ��Subpass��RenderPass
TEST_F(Test_CreateVkRenderPass, CreateWithOneSubpass)
{
	m_Creator.addAttachment(vk::Format::eR16G16B16A16Sfloat);

	SSubPassDescription SubpassDesc;
	SubpassDesc.ColorAttachmentSet = { vk::AttachmentReference {0, vk::ImageLayout::eColorAttachmentOptimal} };
	m_Creator.addSubpass(SubpassDesc);

	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_TRUE(m_VkRenderPass);
	EXPECT_EQ(r, EResult::eSuccess);
}

//***********************************************************************************************
//���Ե�: ������������renderpass
TEST_F(Test_CreateVkRenderPass, CreateTwoRenderpass)
{
	m_Creator.addAttachment(vk::Format::eR8G8B8A8Srgb);
	SSubPassDescription SubpassDesc;
	SubpassDesc.DepthStencilAttachment = { vk::AttachmentReference {0, vk::ImageLayout::eDepthStencilAttachmentOptimal} };
	m_Creator.addSubpass(SubpassDesc);

	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_TRUE(m_VkRenderPass);
	EXPECT_EQ(r, EResult::eSuccess);

	vk::RenderPass Renderpass;
	r = m_Creator.create(Renderpass);
	EXPECT_TRUE(Renderpass);
	EXPECT_EQ(r, EResult::eSuccess);
}

//***********************************************************************************************
//���Ե�: Subpass���Ϸ������
TEST_F(Test_CreateVkRenderPass, CreateWithInvalidSubpass_Case1)
{
	SSubPassDescription SubpassDesc = {};	//�յ�subpass
	m_Creator.addSubpass(SubpassDesc);
	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_FALSE(m_VkRenderPass);
	EXPECT_EQ(r, EResult::eErrorInvalidParameters);
}

//***********************************************************************************************
//���Ե㣺AttachmentReferenceԽ��
TEST_F(Test_CreateVkRenderPass, CreateWithInvalidSubpass_Case2)
{
	m_Creator.addAttachment(vk::Format::eR16G16B16A16Sfloat);

	SSubPassDescription SubpassDesc;
	SubpassDesc.ColorAttachmentSet = { vk::AttachmentReference {2, vk::ImageLayout::eColorAttachmentOptimal} };
	m_Creator.addSubpass(SubpassDesc);

	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_FALSE(m_VkRenderPass);
	EXPECT_EQ(r, EResult::eErrorInvalidParameters);
}

//***********************************************************************************************
//���Ե㣺AttachmentReference Image��ʽ������Ҫ��
TEST_F(Test_CreateVkRenderPass, CreateWithInvalidSubpass_Case3)
{
	m_Creator.addAttachment(vk::Format::eR16G16B16A16Sfloat);

	SSubPassDescription SubpassDesc;
	SubpassDesc.ColorAttachmentSet = { vk::AttachmentReference {2, vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal} };
	m_Creator.addSubpass(SubpassDesc);

	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_FALSE(m_VkRenderPass);
	EXPECT_EQ(r, EResult::eErrorInvalidParameters);
}

//***********************************************************************************************
//���Ե�: �ɹ�����������Subpass��RenderPass
TEST_F(Test_CreateVkRenderPass, CreateWithTwoSubpass)
{
	m_Creator.addAttachment(vk::Format::eR16G16B16A16Sfloat);

	SSubPassDescription SubpassDesc1;
	SubpassDesc1.ColorAttachmentSet = { vk::AttachmentReference {0, vk::ImageLayout::eColorAttachmentOptimal} };
	m_Creator.addSubpass(SubpassDesc1);

	SSubPassDescription SubpassDesc2;
	SubpassDesc2.ColorAttachmentSet = { vk::AttachmentReference {0, vk::ImageLayout::eColorAttachmentOptimal} };
	m_Creator.addSubpass(SubpassDesc2);

	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_TRUE(m_VkRenderPass);
	EXPECT_EQ(r, EResult::eSuccess);
}

//***********************************************************************************************
//���Ե㣺�����ж��attachment��renderpass
TEST_F(Test_CreateVkRenderPass, MultipleAttachments)
{
	m_Creator.addAttachment(vk::Format::eR16G16B16A16Sfloat);
	m_Creator.addAttachment(vk::Format::eR32G32B32A32Sfloat);

	SSubPassDescription SubpassDesc;
	SubpassDesc.ColorAttachmentSet = { vk::AttachmentReference {0, vk::ImageLayout::eColorAttachmentOptimal} };
	SubpassDesc.DepthStencilAttachment = { vk::AttachmentReference { 1, vk::ImageLayout::eDepthStencilAttachmentOptimal} };
	m_Creator.addSubpass(SubpassDesc);

	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_TRUE(m_VkRenderPass);
	EXPECT_EQ(r, EResult::eSuccess);
}

//***********************************************************************************************
//���Ե�:VkContextδ������ʱ������
TEST_F(Test_CreateVkRenderPass, VkContextNotInitialized)
{
	CVkContext::getInstance()->destroyContext();

	m_Creator.addAttachment(vk::Format::eR16G16B16A16Sfloat);

	SSubPassDescription SubpassDesc;
	SubpassDesc.ColorAttachmentSet = { vk::AttachmentReference {0, vk::ImageLayout::eColorAttachmentOptimal} };
	m_Creator.addSubpass(SubpassDesc);

	EResult r = m_Creator.create(m_VkRenderPass);
	EXPECT_FALSE(m_VkRenderPass);
	EXPECT_EQ(r, EResult::eErrorContextNotInitialized);
}