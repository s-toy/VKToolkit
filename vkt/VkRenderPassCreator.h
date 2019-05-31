#pragma once
#include <vector>
#define VULKAN_HPP_DISABLE_ENHANCED_MODE
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	struct SSubPassDescription
	{
		vk::PipelineBindPoint BindPoint = vk::PipelineBindPoint::eGraphics;
		std::vector<vk::AttachmentReference> InputAttachmentSet = {};
		std::vector<vk::AttachmentReference> ColorAttachmentSet = {};
		vk::AttachmentReference DepthStencilAttachment = {};
		vk::AttachmentReference ResolveAttachment = {};
		std::vector<uint32_t>	PreserveAttachmentSet = {};
	};

	class CVkRenderPassCreator
	{
	public:
		vk::Result create(const vk::Device& vDevice, vk::RenderPass& voRenderPass);

		void addAttachment(vk::Format vFormat, vk::ImageLayout vFinalLayout = vk::ImageLayout::eColorAttachmentOptimal, vk::SampleCountFlagBits vSamples = vk::SampleCountFlagBits::e1);

		vk::AttachmentDescription& fetchAttachmentAt(uint32_t vIndex) { _ASSERTE(vIndex < m_AttachmentDescriptionSet.size()); return m_AttachmentDescriptionSet[vIndex]; }
		vk::AttachmentDescription& fetchLastAttachment() { return m_AttachmentDescriptionSet.back(); }

		void addSubpass(const SSubPassDescription& vSubPassDescription);

		void addSubpassDependency(const vk::SubpassDependency& vSubpassDependency) { m_SubpassDependencySet.emplace_back(vSubpassDependency); }

	private:
		vk::RenderPassCreateInfo m_RenderPassCreateInfo;

		std::vector<vk::AttachmentDescription>	m_AttachmentDescriptionSet;
		std::vector<vk::SubpassDescription>		m_SubpassDescriptionSet;
		std::vector<vk::SubpassDependency>		m_SubpassDependencySet;

		void __prepareRenderPassCreateInfo();
	};
}