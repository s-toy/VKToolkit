#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>
#include "VkGraphicsPipelineDefaultState.hpp"

namespace hiveVKT
{
	class CVkGraphicsPipelineCreator
	{
	public:
		CVkGraphicsPipelineCreator() {}

		vk::Pipeline create(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass, uint32_t vSubPass)
		{
			__preparePipelineCreateInfo(vPipelineLayout, vRenderPass, vSubPass);
			return vDevice.createGraphicsPipeline(vPipelineCache, m_PipelineCreateInfo);
		}

		vk::UniquePipeline createUnique(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass, uint32_t vSubPass)
		{
			__preparePipelineCreateInfo(vPipelineLayout, vRenderPass, vSubPass);
			return vDevice.createGraphicsPipelineUnique(vPipelineCache, m_PipelineCreateInfo);
		}

		vk::PipelineInputAssemblyStateCreateInfo& fetchInputAssemblyState() { return m_InputAssemblyStateCreateInfo; }

		vk::PipelineRasterizationStateCreateInfo& fetchRasterizationState() { return m_RasterizationStateCreateInfo; }

		vk::PipelineMultisampleStateCreateInfo& fetchMultisampleState() { return m_MultisampleStateCreateInfo; }

		vk::PipelineDepthStencilStateCreateInfo& fetchDepthStencilState() { return m_DepthStencilStateCreateInfo; }

		vk::PipelineColorBlendStateCreateInfo& fetchColorBlendState() { return m_ColorBlendStateCreateInfo; }

		vk::PipelineTessellationStateCreateInfo& fetchTessellationState() { return m_TessellationStateCreateInfo.value(); }

		void addShaderStage(vk::PipelineShaderStageCreateInfo vShaderStage) { m_ShaderStageSet.emplace_back(vShaderStage); }
		void clearShaderStage() { m_ShaderStageSet.clear(); }

		void addColorBlendAttachment(const vk::PipelineColorBlendAttachmentState& vColorBlendAttachmentState) { m_ColorBlendAttachmentStateSet.emplace_back(vColorBlendAttachmentState); }
		void clearColorBlendAttachment() { m_ColorBlendAttachmentStateSet.clear(); }

		void addVertexBinding(const vk::VertexInputBindingDescription& vValue) { m_VertexBindingDescriptionSet.emplace_back(vValue); }
		void addVertexAttribute(const vk::VertexInputAttributeDescription& vValue) { m_VertexAttributeDescriptionSet.emplace_back(vValue); }
		void clearVertexInputInfo() { m_VertexBindingDescriptionSet.clear(); m_VertexAttributeDescriptionSet.clear(); }

		void addDynamicState(const vk::DynamicState& vDynamicState) { m_DynamicStateSet.emplace_back(vDynamicState); }
		void clearDynamicState() { m_DynamicStateSet.clear(); }

		void addViewport(const vk::Viewport& vValue) { m_ViewportSet.emplace_back(vValue); }
		void addScissor(const vk::Rect2D& vValue) { m_ScissorSet.emplace_back(vValue); }
		void clearViewportAndScissorInfo() { m_ViewportSet.clear(); m_ScissorSet.clear(); }

	private:
		vk::PipelineInputAssemblyStateCreateInfo                m_InputAssemblyStateCreateInfo = DefaultPipelineInputAssemblyStateCreateInfo;
		vk::PipelineVertexInputStateCreateInfo					m_VertexInputStateCreateInfo = DefaultPipelineVertexInputStateCreateInfo;
		vk::PipelineViewportStateCreateInfo						m_ViewportStateCreateInfo = DefaultPipelineViewportStateCreateInfo;
		vk::PipelineRasterizationStateCreateInfo				m_RasterizationStateCreateInfo = DefaultPipelineRasterizationStateCreateInfo;
		vk::PipelineMultisampleStateCreateInfo					m_MultisampleStateCreateInfo = DefaultPipelineMultisampleStateCreateInfo;
		vk::PipelineDepthStencilStateCreateInfo					m_DepthStencilStateCreateInfo = DefaultPipelineDepthStencilStateCreateInfo;
		vk::PipelineColorBlendStateCreateInfo					m_ColorBlendStateCreateInfo = DefaultPipelineColorBlendStateCreateInfo;
		vk::PipelineDynamicStateCreateInfo						m_DynamicStateCreateInfo = DefaultPipelineDynamicStateCreateInfo;

		std::optional<vk::PipelineTessellationStateCreateInfo>	m_TessellationStateCreateInfo;

		std::vector<vk::PipelineShaderStageCreateInfo>			m_ShaderStageSet;
		std::vector<vk::PipelineColorBlendAttachmentState>		m_ColorBlendAttachmentStateSet;
		std::vector<vk::VertexInputAttributeDescription>		m_VertexAttributeDescriptionSet;
		std::vector<vk::VertexInputBindingDescription>			m_VertexBindingDescriptionSet;
		std::vector<vk::DynamicState>							m_DynamicStateSet;
		std::vector<vk::Viewport>                               m_ViewportSet;
		std::vector<vk::Rect2D>                                 m_ScissorSet;

		vk::GraphicsPipelineCreateInfo m_PipelineCreateInfo;

		void __preparePipelineCreateInfo(const vk::PipelineLayout& vPipelineLayout, const vk::RenderPass& vRenderPass, uint32_t vSubPass)
		{
			m_VertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_VertexBindingDescriptionSet.size());
			m_VertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexAttributeDescriptionSet.size());
			m_VertexInputStateCreateInfo.pVertexBindingDescriptions = m_VertexBindingDescriptionSet.data();
			m_VertexInputStateCreateInfo.pVertexAttributeDescriptions = m_VertexAttributeDescriptionSet.data();

			m_ViewportStateCreateInfo.viewportCount = static_cast<uint32_t>(m_ViewportSet.size());
			m_ViewportStateCreateInfo.scissorCount = static_cast<uint32_t>(m_ScissorSet.size());
			m_ViewportStateCreateInfo.pViewports = m_ViewportSet.data();
			m_ViewportStateCreateInfo.pScissors = m_ScissorSet.data();

			m_ColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(m_ColorBlendAttachmentStateSet.size());
			m_ColorBlendStateCreateInfo.pAttachments = m_ColorBlendAttachmentStateSet.data();

			m_DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_DynamicStateSet.size());
			m_DynamicStateCreateInfo.pDynamicStates = m_DynamicStateSet.size() == 0 ? nullptr : m_DynamicStateSet.data();

			m_PipelineCreateInfo.stageCount = static_cast<uint32_t>(m_ShaderStageSet.size());
			m_PipelineCreateInfo.pStages = m_ShaderStageSet.data();
			m_PipelineCreateInfo.pVertexInputState = &m_VertexInputStateCreateInfo;
			m_PipelineCreateInfo.pInputAssemblyState = &m_InputAssemblyStateCreateInfo;
			m_PipelineCreateInfo.pTessellationState = m_TessellationStateCreateInfo.has_value() ? &m_TessellationStateCreateInfo.value() : nullptr;
			m_PipelineCreateInfo.pViewportState = &m_ViewportStateCreateInfo;
			m_PipelineCreateInfo.pRasterizationState = &m_RasterizationStateCreateInfo;
			m_PipelineCreateInfo.pMultisampleState = &m_MultisampleStateCreateInfo;
			m_PipelineCreateInfo.pDepthStencilState = &m_DepthStencilStateCreateInfo;
			m_PipelineCreateInfo.pColorBlendState = &m_ColorBlendStateCreateInfo;
			m_PipelineCreateInfo.pDynamicState = m_DynamicStateSet.empty() ? nullptr : &m_DynamicStateCreateInfo;
			m_PipelineCreateInfo.layout = vPipelineLayout;
			m_PipelineCreateInfo.renderPass = vRenderPass;
			m_PipelineCreateInfo.subpass = vSubPass;
		}
	};
}