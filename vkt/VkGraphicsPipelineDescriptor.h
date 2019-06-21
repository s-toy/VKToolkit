#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>
#include "VkGraphicsPipelineDefaultState.h"
#include "Common.h"
#include "Export.h"

#define _ASSERT_ACTIVE _ASSERTE(m_IsActive, "This function can only be called after CVkGraphicsPipelineDescriptor::begin()."); 

namespace hiveVKT
{
	class VKT_DECLSPEC CVkGraphicsPipelineDescriptor
	{
	public:
		void begin(const vk::Extent2D& vExtent);
		bool end();

		vk::PipelineInputAssemblyStateCreateInfo& fetchInputAssemblyState() { _ASSERT_ACTIVE; return m_InputAssemblyStateCreateInfo; }
		vk::PipelineRasterizationStateCreateInfo& fetchRasterizationState() { _ASSERT_ACTIVE; return m_RasterizationStateCreateInfo; }
		vk::PipelineMultisampleStateCreateInfo& fetchMultisampleState() { _ASSERT_ACTIVE; return m_MultisampleStateCreateInfo; }
		vk::PipelineDepthStencilStateCreateInfo& fetchDepthStencilState() { _ASSERT_ACTIVE; return m_DepthStencilStateCreateInfo; }
		vk::PipelineColorBlendStateCreateInfo& fetchColorBlendState() { _ASSERT_ACTIVE; return m_ColorBlendStateCreateInfo; }
		vk::PipelineTessellationStateCreateInfo& fetchTessellationState() { _ASSERT_ACTIVE; return m_TessellationStateCreateInfo.value(); }

		void addShaderStage(vk::PipelineShaderStageCreateInfo vShaderStage) { _ASSERT_ACTIVE; m_ShaderStageSet.emplace_back(vShaderStage); }

		void addColorBlendAttachment(const vk::PipelineColorBlendAttachmentState& vColorBlendAttachmentState) { _ASSERT_ACTIVE; m_ColorBlendAttachmentStateSet.emplace_back(vColorBlendAttachmentState); }

		void addVertexBinding(const vk::VertexInputBindingDescription& vValue) { _ASSERT_ACTIVE; m_VertexBindingDescriptionSet.emplace_back(vValue); }
		void addVertexAttribute(const vk::VertexInputAttributeDescription& vValue) { _ASSERT_ACTIVE; m_VertexAttributeDescriptionSet.emplace_back(vValue); }

		void addDynamicState(const vk::DynamicState& vDynamicState) { _ASSERT_ACTIVE; m_DynamicStateSet.emplace_back(vDynamicState); }

		void addViewport(const vk::Viewport& vValue) { _ASSERT_ACTIVE; m_ViewportSet.emplace_back(vValue); }
		void addScissor(const vk::Rect2D& vValue) { _ASSERT_ACTIVE; m_ScissorSet.emplace_back(vValue); }

		void setRenderPass(vk::RenderPass vRenderPass) { _ASSERT_ACTIVE; m_PipelineCreateInfo.renderPass = vRenderPass; }
		void setSubpass(uint32_t vSubpass) { _ASSERT_ACTIVE; m_PipelineCreateInfo.subpass = vSubpass; }
		void setPipelineLayout(vk::PipelineLayout vLayout) { _ASSERT_ACTIVE; m_PipelineCreateInfo.layout = vLayout; }

	protected:
		bool _isValid() const { return m_IsCreateInfoValid; }
		const vk::GraphicsPipelineCreateInfo& _getPipelineCreateInfo() const { return m_PipelineCreateInfo; }

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

		bool m_IsCreateInfoValid = false;
		bool m_IsActive = false;

		void __init(const vk::Extent2D& vExtent);
		void __assemblingPipelineCreateInfo();
		void __clearPipelineCreateInfo();

		bool __checkCreateInfoValidity();

		friend class CVkObjectCreator;
	};
}