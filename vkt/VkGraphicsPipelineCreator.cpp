#include "VkGraphicsPipelineCreator.h"

using namespace std;
using namespace hiveVKT;
//***********************************************************************************************
//FUNCTION:
//vk::Pipeline hiveVKT::CVkGraphicsPipelineCreator::create(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass, uint32_t vSubPass)
//{
//	__preparePipelineCreateInfo(vPipelineLayout, vRenderPass, vSubPass);
//	return vDevice.createGraphicsPipeline(vPipelineCache, m_PipelineCreateInfo);
//}

EResult hiveVKT::CVkGraphicsPipelineCreator::create(vk::Pipeline & voPipeline, const vk::Device & vDevice, const vk::PipelineLayout & vPipelineLayout, const vk::PipelineCache & vPipelineCache, const vk::RenderPass & vRenderPass, uint32_t vSubPass)

{
	if (__isParameterWrong(vDevice,vPipelineLayout,vRenderPass,vSubPass))
		return EResult::eErrorInvalidParameters;
	if (__isPipelineSettingWrong())
		return EResult::eErrorInitializationFailed;
	__preparePipelineCreateInfo(vPipelineLayout, vRenderPass, vSubPass);
	return static_cast<EResult>(vDevice.createGraphicsPipelines(vPipelineCache, 1, &m_PipelineCreateInfo, nullptr, &voPipeline));
}

//***********************************************************************************************
//FUNCTION:
vk::UniquePipeline hiveVKT::CVkGraphicsPipelineCreator::createUnique(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass, uint32_t vSubPass)
{
	__preparePipelineCreateInfo(vPipelineLayout, vRenderPass, vSubPass);
	return vDevice.createGraphicsPipelineUnique(vPipelineCache, m_PipelineCreateInfo);
}

bool hiveVKT::CVkGraphicsPipelineCreator::__isParameterWrong(const vk::Device & vDevice, const vk::PipelineLayout & vPipelineLayout, 
	const vk::RenderPass & vRenderPass, uint32_t vSubPass)
{
		//!=��==�������������Ƶ�ת��
	if (/*vDevice != NULL && vPipelineLayout != VK_NULL_HANDLE && vRenderPass != VK_NULL_HANDLE &&*/ vSubPass == 0 )
		return false;
	else
	{
		return true;
	}
}

bool hiveVKT::CVkGraphicsPipelineCreator::__isPipelineSettingWrong()
{
	if (m_ShaderStageSet.size() <= 1 || m_ViewportSet.size() <= 0 || m_ScissorSet.size() <= 0
		|| m_ColorBlendAttachmentStateSet.size() <= 0)
		return true;
	else
	{
		return false;
	}
}

bool hiveVKT::CVkGraphicsPipelineCreator::__isShaderStageWrong()
{
	/*if (m_ShaderStageSet.size() >= 2)
	{
		if(m_ShaderStageSet[0].)
	}*/
	return false;
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGraphicsPipelineCreator::__preparePipelineCreateInfo(const vk::PipelineLayout& vPipelineLayout, const vk::RenderPass& vRenderPass, uint32_t vSubPass)
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
