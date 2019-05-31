#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>
#include "VkGraphicsPipelineDefaultState.h"

namespace hiveVKT
{
	class CVkGraphicsPipelineCreator
	{
	public:
		CVkGraphicsPipelineCreator() {}
		 

		//vk::Pipeline create(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass, uint32_t vSubPass);
		vk::Result create(vk::Pipeline& voPipeline,const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass, uint32_t vSubPass);
		vk::UniquePipeline createUnique(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass, uint32_t vSubPass);

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

		void __preparePipelineCreateInfo(const vk::PipelineLayout& vPipelineLayout, const vk::RenderPass& vRenderPass, uint32_t vSubPass);
	};
}