#include "VkRenderPassCreator.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::RenderPass hiveVKT::CVkRenderPassCreator::create(const vk::Device& vDevice)
{
	__prepareRenderPassCreateInfo();
	return vDevice.createRenderPass(m_RenderPassCreateInfo);
}

//***********************************************************************************************
//FUNCTION:
vk::UniqueRenderPass hiveVKT::CVkRenderPassCreator::createUnique(const vk::Device& vDevice)
{
	__prepareRenderPassCreateInfo();
	return vDevice.createRenderPassUnique(m_RenderPassCreateInfo);
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
void hiveVKT::CVkRenderPassCreator::addSubpass(const std::vector<vk::AttachmentReference>& vColorAttachmentReferences, 
	const vk::AttachmentReference& vDepthStencilAttachmentReference, 
	const std::vector<vk::AttachmentReference>& vResolveAttachmentReferences, 
	const std::vector<vk::AttachmentReference>& vInputAttachmentReferences, 
	const std::vector<uint32_t>& vPreserveAttachmentReferences, 
	vk::PipelineBindPoint vBindPoint /*= vk::PipelineBindPoint::eGraphics*/)
{
	vk::SubpassDescription SubpassDesc = {};
	SubpassDesc.pipelineBindPoint = vBindPoint;
	SubpassDesc.colorAttachmentCount = static_cast<uint32_t>(vColorAttachmentReferences.size());
	SubpassDesc.pColorAttachments = vColorAttachmentReferences.data();
	SubpassDesc.inputAttachmentCount = static_cast<uint32_t>(vInputAttachmentReferences.size());
	SubpassDesc.pInputAttachments = vInputAttachmentReferences.data();
	SubpassDesc.preserveAttachmentCount = static_cast<uint32_t>(vPreserveAttachmentReferences.size());
	SubpassDesc.pPreserveAttachments = vPreserveAttachmentReferences.data();
	SubpassDesc.pDepthStencilAttachment = (vDepthStencilAttachmentReference == vk::AttachmentReference()) ? nullptr : &vDepthStencilAttachmentReference;
	SubpassDesc.pResolveAttachments = vResolveAttachmentReferences.data();
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