#include "VkGraphicsPipelineDescriptor.h"
#include "VkInitializer.h"

using namespace std;
using namespace hiveVKT;

CVkGraphicsPipelineDescriptor::CVkGraphicsPipelineDescriptor(const vk::Extent2D& vExtent)
{
	__init(vExtent);
}

CVkGraphicsPipelineDescriptor::~CVkGraphicsPipelineDescriptor()
{
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
	m_InputAssemblyStateCreateInfo = initializer::pipelineInputAssemblyStateCreateInfo();
	m_VertexInputStateCreateInfo = {};
	m_ViewportStateCreateInfo = {};
	m_RasterizationStateCreateInfo = initializer::pipelineRasterizationStateCreateInfo();
	m_MultisampleStateCreateInfo = initializer::pipelineMultisampleStateCreateInfo();
	m_DepthStencilStateCreateInfo = initializer::pipelineDepthStencilStateCreateInfo();
	m_ColorBlendStateCreateInfo = initializer::pipelineColorBlendStateCreateInfo();
	m_DynamicStateCreateInfo = initializer::pipelineDynamicStateCreateInfo();

	addColorBlendAttachment(initializer::pipelineColorBlendAttachmentState());
	addViewport(initializer::viewport(vExtent.width, vExtent.height));
	addScissor(initializer::scissor(vExtent.width, vExtent.height));
}

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::CVkGraphicsPipelineDescriptor::__checkCreateInfoValidity() const
{
	if (m_ShaderStageSet.size() <= 1 || m_ViewportSet.size() <= 0 || m_ScissorSet.size() <= 0 || m_ColorBlendAttachmentStateSet.size() <= 0)
		return false;

	return true;
}