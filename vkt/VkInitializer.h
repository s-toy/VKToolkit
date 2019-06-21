#pragma once
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	namespace initializer
	{
		inline vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo()
		{
			vk::PipelineShaderStageCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineShaderStageCreateFlags();
			CreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
			CreateInfo.module = vk::ShaderModule();
			CreateInfo.pName = nullptr;
			CreateInfo.pSpecializationInfo = nullptr;

			return CreateInfo;
		}

		inline vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo()
		{
			vk::PipelineVertexInputStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineVertexInputStateCreateFlags();
			CreateInfo.vertexBindingDescriptionCount = 0;
			CreateInfo.pVertexBindingDescriptions = nullptr;
			CreateInfo.vertexAttributeDescriptionCount = 0;
			CreateInfo.pVertexAttributeDescriptions = nullptr;

			return CreateInfo;
		}

		inline vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo()
		{
			vk::PipelineInputAssemblyStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
			CreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
			CreateInfo.primitiveRestartEnable = 0;

			return CreateInfo;
		}

		inline vk::PipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo()
		{
			vk::PipelineTessellationStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineTessellationStateCreateFlags();
			CreateInfo.patchControlPoints = 0;

			return CreateInfo;
		}

		inline vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(float vWidth, float vHeight)
		{
			vk::PipelineViewportStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineViewportStateCreateFlags();
			CreateInfo.viewportCount = 1;
			CreateInfo.pViewports = &vk::Viewport(0, 0, vWidth, vHeight, 0, 1);
			CreateInfo.scissorCount = 1;
			CreateInfo.pScissors = &vk::Rect2D({ 0, 0 }, {vWidth, vHeight});

			return CreateInfo;
		}

		inline vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo()
		{
			vk::PipelineRasterizationStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineRasterizationStateCreateFlags();
			CreateInfo.depthClampEnable = 0;
			CreateInfo.rasterizerDiscardEnable = 0;
			CreateInfo.polygonMode = vk::PolygonMode::eFill;
			CreateInfo.cullMode = vk::CullModeFlags();
			CreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
			CreateInfo.depthBiasEnable = 0;
			CreateInfo.depthBiasConstantFactor = 0;
			CreateInfo.depthBiasClamp = 0;
			CreateInfo.depthBiasSlopeFactor = 0;
			CreateInfo.lineWidth = 0;
			return CreateInfo;
		}

		inline vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo()
		{
			vk::PipelineMultisampleStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineMultisampleStateCreateFlags();
			CreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
			CreateInfo.sampleShadingEnable = 0;
			CreateInfo.minSampleShading = 0;
			CreateInfo.pSampleMask = nullptr;
			CreateInfo.alphaToCoverageEnable = 0;
			CreateInfo.alphaToOneEnable = 0;
			return CreateInfo;
		}

		inline vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo()
		{
			vk::PipelineDepthStencilStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineDepthStencilStateCreateFlags();
			CreateInfo.depthTestEnable = 0;
			CreateInfo.depthWriteEnable = 0;
			CreateInfo.depthCompareOp = vk::CompareOp::eNever;
			CreateInfo.depthBoundsTestEnable = 0;
			CreateInfo.stencilTestEnable = 0;
			CreateInfo.front = vk::StencilOpState();
			CreateInfo.back = vk::StencilOpState();
			CreateInfo.minDepthBounds = 0;
			CreateInfo.maxDepthBounds = 0;
			return CreateInfo;
		}

		inline vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo()
		{
			vk::PipelineColorBlendStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineColorBlendStateCreateFlags();
			CreateInfo.logicOpEnable = 0;
			CreateInfo.logicOp = vk::LogicOp::eClear;
			CreateInfo.attachmentCount = 0;
			CreateInfo.pAttachments = nullptr;
			//CreateInfo.blendConstants = { { 0, 0, 0, 0 } } ;
			return CreateInfo;
		}

		inline vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo()
		{
			vk::PipelineDynamicStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineDynamicStateCreateFlags();
			CreateInfo.dynamicStateCount = 0;
			CreateInfo.pDynamicStates = nullptr;
			return CreateInfo;
		}

		inline vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo()
		{
			vk::GraphicsPipelineCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineCreateFlags();
			CreateInfo.stageCount = 0;
			CreateInfo.pStages = nullptr;
			CreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo();
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

			return CreateInfo;
		}
	}
}