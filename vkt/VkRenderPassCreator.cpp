#include "VkRenderPassCreator.h"
#include "common/CommonMicro.h"
#include "common/UtilityInterface.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
EResult hiveVKT::CVkRenderPassCreator::create(const vk::Device& vDevice, vk::RenderPass& voRenderPass)
{
	bool IsParamsValid = true;

	if (!vDevice) {
		IsParamsValid = false;
		_OUTPUT_WARNING("The input paramter [vDevice] must be a valid vk::Device handle.")
	}
	if (m_SubpassDescriptionSet.size() < 1) {
		IsParamsValid = false;
		_OUTPUT_WARNING("A render pass must have at least one subpass, call CVkRenderPassCreator::addSubpass() to add a subpass.")
	}

	if (!IsParamsValid) { voRenderPass = nullptr; return EResult::eErrorInvalidParameters; }

	__prepareRenderPassCreateInfo();
	return static_cast<EResult>(vDevice.createRenderPass(&m_RenderPassCreateInfo, nullptr, &voRenderPass));
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
	SubpassDesc.pResolveAttachments = vSubPassDescription.ResolveAttachment.has_value() ? &vSubPassDescription.ResolveAttachment.value() : nullptr;
	SubpassDesc.pDepthStencilAttachment = vSubPassDescription.DepthStencilAttachment.has_value() ? &vSubPassDescription.DepthStencilAttachment.value() : nullptr;
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