#pragma once
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	class CVkRenderPassCreator
	{
	public:
		vk::RenderPass create(const vk::Device& vDevice);
		vk::UniqueRenderPass createUnique(const vk::Device& vDevice);

		void addAttachment(vk::Format vFormat, vk::ImageLayout vFinalLayout = vk::ImageLayout::eColorAttachmentOptimal, vk::SampleCountFlagBits vSamples = vk::SampleCountFlagBits::e1);

		vk::AttachmentDescription& fetchAttachmentAt(uint32_t vIndex) { _ASSERTE(vIndex < m_AttachmentDescriptionSet.size()); return m_AttachmentDescriptionSet[vIndex]; }
		vk::AttachmentDescription& fetchLastAttachment() { return m_AttachmentDescriptionSet.back(); }

		void addSubpass(const std::vector<vk::AttachmentReference>& vColorAttachmentReferences,
			const vk::AttachmentReference& vDepthStencilAttachmentReference,
			const std::vector<vk::AttachmentReference>& vResolveAttachmentReferences,
			const std::vector<vk::AttachmentReference>& vInputAttachmentReferences,
			const std::vector<uint32_t>& vPreserveAttachmentReferences,
			vk::PipelineBindPoint vBindPoint = vk::PipelineBindPoint::eGraphics);

		void addSubpassDependency(const vk::SubpassDependency& vSubpassDependency) { m_SubpassDependencySet.emplace_back(vSubpassDependency); }

	private:
		vk::RenderPassCreateInfo m_RenderPassCreateInfo;

		std::vector<vk::AttachmentDescription>	m_AttachmentDescriptionSet;
		std::vector<vk::SubpassDescription>		m_SubpassDescriptionSet;
		std::vector<vk::SubpassDependency>		m_SubpassDependencySet;

		void __prepareRenderPassCreateInfo();
	};
}