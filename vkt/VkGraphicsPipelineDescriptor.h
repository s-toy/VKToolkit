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
		void begin(const vk::Extent2D& vExtent);
		bool end();

		bool isValid() const { return m_IsCreateInfoValid; }

		const vk::GraphicsPipelineCreateInfo& getPipelineCreateInfo() const { return m_PipelineCreateInfo; }

		vk::PipelineInputAssemblyStateCreateInfo& fetchInputAssemblyState() { _ASSERTE(m_IsActive); return m_InputAssemblyStateCreateInfo; }
		vk::PipelineRasterizationStateCreateInfo& fetchRasterizationState() { _ASSERTE(m_IsActive); return m_RasterizationStateCreateInfo; }
		vk::PipelineMultisampleStateCreateInfo& fetchMultisampleState() { _ASSERTE(m_IsActive); return m_MultisampleStateCreateInfo; }
		vk::PipelineDepthStencilStateCreateInfo& fetchDepthStencilState() { _ASSERTE(m_IsActive); return m_DepthStencilStateCreateInfo; }
		vk::PipelineColorBlendStateCreateInfo& fetchColorBlendState() { _ASSERTE(m_IsActive); return m_ColorBlendStateCreateInfo; }
		vk::PipelineTessellationStateCreateInfo& fetchTessellationState() { _ASSERTE(m_IsActive); return m_TessellationStateCreateInfo.value(); }

		void addShaderStage(vk::PipelineShaderStageCreateInfo vShaderStage) { _ASSERTE(m_IsActive); m_ShaderStageSet.emplace_back(vShaderStage); }
		void clearShaderStage() { _ASSERTE(m_IsActive); m_ShaderStageSet.clear(); }

		void addColorBlendAttachment(const vk::PipelineColorBlendAttachmentState& vColorBlendAttachmentState) { _ASSERTE(m_IsActive); m_ColorBlendAttachmentStateSet.emplace_back(vColorBlendAttachmentState); }
		void clearColorBlendAttachment() { _ASSERTE(m_IsActive); m_ColorBlendAttachmentStateSet.clear(); }

		void addVertexBinding(const vk::VertexInputBindingDescription& vValue) { _ASSERTE(m_IsActive); m_VertexBindingDescriptionSet.emplace_back(vValue); }
		void addVertexAttribute(const vk::VertexInputAttributeDescription& vValue) { _ASSERTE(m_IsActive); m_VertexAttributeDescriptionSet.emplace_back(vValue); }
		void clearVertexInputInfo() { _ASSERTE(m_IsActive); m_VertexBindingDescriptionSet.clear(); m_VertexAttributeDescriptionSet.clear(); }

		void addDynamicState(const vk::DynamicState& vDynamicState) { _ASSERTE(m_IsActive); m_DynamicStateSet.emplace_back(vDynamicState); }
		void clearDynamicState() { _ASSERTE(m_IsActive); m_DynamicStateSet.clear(); }

		void addViewport(const vk::Viewport& vValue) { _ASSERTE(m_IsActive); m_ViewportSet.emplace_back(vValue); }
		void addScissor(const vk::Rect2D& vValue) { _ASSERTE(m_IsActive); m_ScissorSet.emplace_back(vValue); }
		void clearViewportAndScissorInfo() { _ASSERTE(m_IsActive); m_ViewportSet.clear(); m_ScissorSet.clear(); }

		void setRenderPass(vk::RenderPass vRenderPass) { _ASSERTE(m_IsActive); m_PipelineCreateInfo.renderPass = vRenderPass; }
		void setSubpass(uint32_t vSubpass) { _ASSERTE(m_IsActive); m_PipelineCreateInfo.subpass = vSubpass; }
		void setPipelineLayout(vk::PipelineLayout vLayout) { _ASSERTE(m_IsActive); m_PipelineCreateInfo.layout = vLayout; }

	private:
		vk::PipelineInputAssemblyStateCreateInfo                m_InputAssemblyStateCreateInfo;
		vk::PipelineVertexInputStateCreateInfo					m_VertexInputStateCreateInfo;
		vk::PipelineViewportStateCreateInfo						m_ViewportStateCreateInfo;
		vk::PipelineRasterizationStateCreateInfo				m_RasterizationStateCreateInfo;
		vk::PipelineMultisampleStateCreateInfo					m_MultisampleStateCreateInfo;
		vk::PipelineDepthStencilStateCreateInfo					m_DepthStencilStateCreateInfo;
		vk::PipelineColorBlendStateCreateInfo					m_ColorBlendStateCreateInfo;
		vk::PipelineDynamicStateCreateInfo						m_DynamicStateCreateInfo;

		std::optional<vk::PipelineTessellationStateCreateInfo>	m_TessellationStateCreateInfo;

		std::vector<vk::PipelineShaderStageCreateInfo>			m_ShaderStageSet;
		std::vector<vk::PipelineColorBlendAttachmentState>		m_ColorBlendAttachmentStateSet;
		std::vector<vk::VertexInputAttributeDescription>		m_VertexAttributeDescriptionSet;
		std::vector<vk::VertexInputBindingDescription>			m_VertexBindingDescriptionSet;
		std::vector<vk::DynamicState>							m_DynamicStateSet;
		std::vector<vk::Viewport>                               m_ViewportSet;
		std::vector<vk::Rect2D>                                 m_ScissorSet;

		vk::GraphicsPipelineCreateInfo m_PipelineCreateInfo;

		bool m_IsCreateInfoValid = false;
		bool m_IsActive = false;

		bool __isParameterWrong(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout,
			const vk::RenderPass& vRenderPass, uint32_t vSubPass);

		bool __isPipelineSettingWrong();
		bool __isShaderStageWrong();

		void __constructDefaultPipelineCreateInfo(const vk::Extent2D& vExtent);
		void __preparePipelineCreateInfo();
		void __clearPipelineCreateInfo();
		bool __checkCreateInfoValidity();
	};
}