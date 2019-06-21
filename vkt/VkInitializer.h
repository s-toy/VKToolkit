#pragma once
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	namespace initializer
	{
		inline vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo()
		{
			vk::PipelineShaderStageCreateInfo CreateInfo = {};

		}

		inline vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo()
		{
			vk::GraphicsPipelineCreateInfo CreateInfo = {};
			CreateInfo.stageCount = 0;
			CreateInfo.pStages = nullptr;
			CreateInfo.pVertexInputState = nullptr;
			CreateInfo.pInputAssemblyState = nullptr;
			CreateInfo.pTessellationState = nullptr;
			CreateInfo.pViewportState = nullptr;
			CreateInfo.pRasterizationState = nullptr;
			CreateInfo.pMultisampleState = nullptr;
			CreateInfo.pDepthStencilState = nullptr;
			CreateInfo.pColorBlendState = nullptr;
			CreateInfo.pDynamicState = nullptr;
			CreateInfo.layout = vk::PipelineLayout();
			CreateInfo.renderPass = vk::RenderPass();
			CreateInfo.subpass = 0;
			CreateInfo.basePipelineHandle = vk::Pipeline();
			CreateInfo.basePipelineIndex = 0;
		}
	}
}