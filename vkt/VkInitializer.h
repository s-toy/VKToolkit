#pragma once
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	namespace initializer
	{
		inline vk::ApplicationInfo applicationInfo()
		{
			vk::ApplicationInfo AppInfo = {};
			AppInfo.pApplicationName = "HiveApplication";
			AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			AppInfo.pEngineName = "HiveVKT";
			AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			AppInfo.apiVersion = VK_API_VERSION_1_1;
			return AppInfo;
		}

		inline vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(const vk::ShaderModule& vShaderModule, vk::ShaderStageFlagBits vStage = vk::ShaderStageFlagBits::eVertex)
		{
			vk::PipelineShaderStageCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineShaderStageCreateFlags();
			CreateInfo.stage = vStage;
			CreateInfo.module = vShaderModule;
			CreateInfo.pName = "main";
			CreateInfo.pSpecializationInfo = nullptr;
			return CreateInfo;
		}

		inline vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
			const std::vector<vk::VertexInputBindingDescription>& vBindingDescriptions,
			const std::vector<vk::VertexInputAttributeDescription>& vAttributeDescriptions)
		{
			vk::PipelineVertexInputStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineVertexInputStateCreateFlags();
			CreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vBindingDescriptions.size());
			CreateInfo.pVertexBindingDescriptions = vBindingDescriptions.data();
			CreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vAttributeDescriptions.size());;
			CreateInfo.pVertexAttributeDescriptions = vAttributeDescriptions.data();
			return CreateInfo;
		}

		inline vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo()
		{
			vk::PipelineInputAssemblyStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
			CreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
			CreateInfo.primitiveRestartEnable = false;
			return CreateInfo;
		}

		inline vk::PipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo()
		{
			vk::PipelineTessellationStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineTessellationStateCreateFlags();
			CreateInfo.patchControlPoints = 0;
			return CreateInfo;
		}

		inline vk::Viewport viewport(float vWidth, float vHeight)
		{
			return vk::Viewport(0, 0, vWidth, vHeight, 0, 1);
		}

		inline vk::Rect2D scissor(uint32_t vWidth, uint32_t vHeight)
		{
			return vk::Rect2D({ 0, 0 }, { vWidth, vHeight });
		}

		inline vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(const vk::Viewport& vViewport, const vk::Rect2D& vScissor)
		{
			vk::PipelineViewportStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineViewportStateCreateFlags();
			CreateInfo.viewportCount = 1;
			CreateInfo.pViewports = &vViewport;
			CreateInfo.scissorCount = 1;
			CreateInfo.pScissors = &vScissor;
			return CreateInfo;
		}

		inline vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo()
		{
			vk::PipelineRasterizationStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineRasterizationStateCreateFlags();
			CreateInfo.depthClampEnable = false;
			CreateInfo.rasterizerDiscardEnable = false;
			CreateInfo.polygonMode = vk::PolygonMode::eFill;
			CreateInfo.cullMode = vk::CullModeFlagBits::eBack;
			CreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
			CreateInfo.depthBiasEnable = false;
			CreateInfo.depthBiasConstantFactor = 0;
			CreateInfo.depthBiasClamp = 0;
			CreateInfo.depthBiasSlopeFactor = 0;
			CreateInfo.lineWidth = 1.0;
			return CreateInfo;
		}

		inline vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo()
		{
			vk::PipelineMultisampleStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineMultisampleStateCreateFlags();
			CreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
			CreateInfo.sampleShadingEnable = false;
			CreateInfo.minSampleShading = 0;
			CreateInfo.pSampleMask = nullptr;
			CreateInfo.alphaToCoverageEnable = false;
			CreateInfo.alphaToOneEnable = false;
			return CreateInfo;
		}

		inline vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo()
		{
			vk::PipelineDepthStencilStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineDepthStencilStateCreateFlags();
			CreateInfo.depthTestEnable = false;
			CreateInfo.depthWriteEnable = false;
			CreateInfo.depthCompareOp = vk::CompareOp::eNever;
			CreateInfo.depthBoundsTestEnable = false;
			CreateInfo.stencilTestEnable = false;
			CreateInfo.front = vk::StencilOpState();
			CreateInfo.back = vk::StencilOpState();
			CreateInfo.minDepthBounds = 0.0;
			CreateInfo.maxDepthBounds = 1.0;
			return CreateInfo;
		}

		inline vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState()
		{
			vk::PipelineColorBlendAttachmentState AttachmentState = {};
			AttachmentState.blendEnable = false;
			AttachmentState.srcColorBlendFactor = vk::BlendFactor::eOne;
			AttachmentState.dstColorBlendFactor = vk::BlendFactor::eZero;
			AttachmentState.colorBlendOp = vk::BlendOp::eAdd;
			AttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
			AttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
			AttachmentState.alphaBlendOp = vk::BlendOp::eAdd;
			AttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
			return AttachmentState;
		}

		inline vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo()
		{
			vk::PipelineColorBlendStateCreateInfo CreateInfo = {};
			CreateInfo.flags = vk::PipelineColorBlendStateCreateFlags();
			CreateInfo.logicOpEnable = false;
			CreateInfo.logicOp = vk::LogicOp::eClear;
			CreateInfo.attachmentCount = 0;
			CreateInfo.pAttachments = nullptr;
			CreateInfo.blendConstants[0] = 0.0;
			CreateInfo.blendConstants[1] = 0.0;
			CreateInfo.blendConstants[2] = 0.0;
			CreateInfo.blendConstants[3] = 0.0;
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
			return CreateInfo;
		}
	}
}