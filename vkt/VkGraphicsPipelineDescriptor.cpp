#include "VkGraphicsPipelineDescriptor.h"

using namespace std;
using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGraphicsPipelineDescriptor::begin(const vk::Extent2D& vExtent)
{
	__clearPipelineCreateInfo();
	__init(vExtent);

	m_IsActive = true;
}

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::CVkGraphicsPipelineDescriptor::end()
{
	_ASSERTE(m_IsActive);

	m_IsCreateInfoValid = __checkCreateInfoValidity();
	if (!m_IsCreateInfoValid) return false;

	__assemblingPipelineCreateInfo();

	m_IsActive = false;

	return true;
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGraphicsPipelineDescriptor::__assemblingPipelineCreateInfo()
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
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGraphicsPipelineDescriptor::__init(const vk::Extent2D& vExtent)
{
	m_InputAssemblyStateCreateInfo = DefaultPipelineInputAssemblyStateCreateInfo;
	m_VertexInputStateCreateInfo = DefaultPipelineVertexInputStateCreateInfo;
	m_ViewportStateCreateInfo = DefaultPipelineViewportStateCreateInfo;
	m_RasterizationStateCreateInfo = DefaultPipelineRasterizationStateCreateInfo;
	m_MultisampleStateCreateInfo = DefaultPipelineMultisampleStateCreateInfo;
	m_DepthStencilStateCreateInfo = DefaultPipelineDepthStencilStateCreateInfo;
	m_ColorBlendStateCreateInfo = DefaultPipelineColorBlendStateCreateInfo;
	m_DynamicStateCreateInfo = DefaultPipelineDynamicStateCreateInfo;

	addColorBlendAttachment(DefaultPipelineColorBlendAttachmentState);
	addViewport(vk::Viewport(0, 0, vExtent.width, vExtent.height, 0, 1.0));
	addScissor(vk::Rect2D(vk::Offset2D(0, 0), vExtent));
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGraphicsPipelineDescriptor::__clearPipelineCreateInfo()
{
	m_ShaderStageSet.clear();
	m_ColorBlendAttachmentStateSet.clear();
	m_VertexAttributeDescriptionSet.clear();
	m_VertexBindingDescriptionSet.clear();
	m_DynamicStateSet.clear();
	m_ViewportSet.clear();
	m_ScissorSet.clear();
}

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::CVkGraphicsPipelineDescriptor::__checkCreateInfoValidity()
{
	if (m_ShaderStageSet.size() <= 1 || m_ViewportSet.size() <= 0 || m_ScissorSet.size() <= 0
		|| m_ColorBlendAttachmentStateSet.size() <= 0)
		return true;
	else
	{
		return false;
	}

	return true;
}