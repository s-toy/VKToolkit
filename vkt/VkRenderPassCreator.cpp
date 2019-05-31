#include "VkRenderPassCreator.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::Result hiveVKT::CVkRenderPassCreator::create(const vk::Device& vDevice, vk::RenderPass& voRenderPass)
{
	if (m_SubpassDescriptionSet.size() < 1)
	{
		voRenderPass = nullptr;
		return vk::Result::eIncomplete;
	}

	__prepareRenderPassCreateInfo();
	return vDevice.createRenderPass(&m_RenderPassCreateInfo, nullptr, &voRenderPass);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkRenderPassCreator::addAttachment(vk::Format vFormat, vk::ImageLayout vFinalLayout /*= vk::ImageLayout::eColorAttachmentOptimal*/, vk::SampleCountFlagBits vSamples /*= vk::SampleCountFlagBits::e1*/)
{
	vk::AttachmentDescription AttachmentDesc = {};
	AttachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
	AttachmentDesc.finalLayout = vFinalLayout;
	AttachmentDesc.format = vFormat;
	AttachmentDesc.samples = vSamples;
	AttachmentDesc.loadOp = vk::AttachmentLoadOp::eClear;
	AttachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
	AttachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	AttachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	m_AttachmentDescriptionSet.emplace_back(AttachmentDesc);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkRenderPassCreator::addSubpass(const SSubPassDescription& vSubPassDescription)
{
	vk::SubpassDescription SubpassDesc = {};
	SubpassDesc.pipelineBindPoint = vSubPassDescription.BindPoint;
	SubpassDesc.inputAttachmentCount = static_cast<uint32_t>(vSubPassDescription.InputAttachmentSet.size());
	SubpassDesc.pInputAttachments = vSubPassDescription.InputAttachmentSet.data();
	SubpassDesc.colorAttachmentCount = static_cast<uint32_t>(vSubPassDescription.ColorAttachmentSet.size());
	SubpassDesc.pColorAttachments = vSubPassDescription.ColorAttachmentSet.data();
	SubpassDesc.pResolveAttachments = &vSubPassDescription.ResolveAttachment;
	SubpassDesc.pDepthStencilAttachment = &vSubPassDescription.DepthStencilAttachment;
	SubpassDesc.preserveAttachmentCount = static_cast<uint32_t>(vSubPassDescription.PreserveAttachmentSet.size());
	SubpassDesc.pPreserveAttachments = vSubPassDescription.PreserveAttachmentSet.data();
	m_SubpassDescriptionSet.push_back(SubpassDesc);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkRenderPassCreator::__prepareRenderPassCreateInfo()
{
	m_RenderPassCreateInfo.attachmentCount = static_cast<uint32_t>(m_AttachmentDescriptionSet.size());
	m_RenderPassCreateInfo.pAttachments = m_AttachmentDescriptionSet.data();
	m_RenderPassCreateInfo.subpassCount = static_cast<uint32_t>(m_SubpassDescriptionSet.size());
	m_RenderPassCreateInfo.pSubpasses = m_SubpassDescriptionSet.data();
	m_RenderPassCreateInfo.dependencyCount = static_cast<uint32_t>(m_SubpassDependencySet.size());
	m_RenderPassCreateInfo.pDependencies = m_SubpassDependencySet.data();
}