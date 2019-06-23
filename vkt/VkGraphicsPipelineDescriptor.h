#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>
#include "VkGraphicsPipelineDefaultState.h"
#include "Common.h"
#include "Export.h"

namespace hiveVKT
{
	class VKT_DECLSPEC CVkGraphicsPipelineDescriptor
	{
	public:
		CVkGraphicsPipelineDescriptor(const vk::Extent2D& vExtent);
		~CVkGraphicsPipelineDescriptor();

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

		void setRenderPass(vk::RenderPass vRenderPass) { m_PipelineCreateInfo.renderPass = vRenderPass; }
		void setSubpass(uint32_t vSubpass) { m_PipelineCreateInfo.subpass = vSubpass; }
		void setPipelineLayout(vk::PipelineLayout vLayout) { m_PipelineCreateInfo.layout = vLayout; }

	protected:
		bool _isValid() const { return __checkCreateInfoValidity(); }
		const vk::GraphicsPipelineCreateInfo& _getPipelineCreateInfo() { __assemblingPipelineCreateInfo(); return m_PipelineCreateInfo; }

	private:
		vk::PipelineInputAssemblyStateCreateInfo                m_InputAssemblyStateCreateInfo = {};
		vk::PipelineVertexInputStateCreateInfo					m_VertexInputStateCreateInfo = {};
		vk::PipelineViewportStateCreateInfo						m_ViewportStateCreateInfo = {};
		vk::PipelineRasterizationStateCreateInfo				m_RasterizationStateCreateInfo = {};
		vk::PipelineMultisampleStateCreateInfo					m_MultisampleStateCreateInfo = {};
		vk::PipelineDepthStencilStateCreateInfo					m_DepthStencilStateCreateInfo = {};
		vk::PipelineColorBlendStateCreateInfo					m_ColorBlendStateCreateInfo = {};
		vk::PipelineDynamicStateCreateInfo						m_DynamicStateCreateInfo = {};

		std::optional<vk::PipelineTessellationStateCreateInfo>	m_TessellationStateCreateInfo;

		std::vector<vk::PipelineShaderStageCreateInfo>			m_ShaderStageSet;
		std::vector<vk::PipelineColorBlendAttachmentState>		m_ColorBlendAttachmentStateSet;
		std::vector<vk::VertexInputAttributeDescription>		m_VertexAttributeDescriptionSet;
		std::vector<vk::VertexInputBindingDescription>			m_VertexBindingDescriptionSet;
		std::vector<vk::DynamicState>							m_DynamicStateSet;
		std::vector<vk::Viewport>                               m_ViewportSet;
		std::vector<vk::Rect2D>                                 m_ScissorSet;

		vk::GraphicsPipelineCreateInfo m_PipelineCreateInfo = {};

		void __init(const vk::Extent2D& vExtent);
		void __assemblingPipelineCreateInfo();
		bool __checkCreateInfoValidity() const;

		friend class CVkObjectCreator;
	};
}