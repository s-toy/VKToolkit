#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	class CVkGraphicsPipelineCreator
	{
	public:
		CVkGraphicsPipelineCreator(uint32_t vWidth, uint32_t vHeight)
		{
			__initPipeline(vWidth, vHeight);
		}

		vk::Pipeline create(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass)
		{
			__preparePipelineCreateInfo(vPipelineLayout, vRenderPass);
			return vDevice.createGraphicsPipeline(vPipelineCache, m_PipelineCreateInfo);
		}

		vk::UniquePipeline createUnique(const vk::Device& vDevice, const vk::PipelineLayout& vPipelineLayout, const vk::PipelineCache& vPipelineCache, const vk::RenderPass& vRenderPass)
		{
			__preparePipelineCreateInfo(vPipelineLayout, vRenderPass);
			return vDevice.createGraphicsPipelineUnique(vPipelineCache, m_PipelineCreateInfo);
		}

		void addShaderStage(vk::ShaderStageFlagBits vStageFlag, const vk::ShaderModule& vModule, const char* vEntryName = "main")
		{
			vk::PipelineShaderStageCreateInfo ShaderStage{ {}, vStageFlag , vModule, vEntryName };
			m_ShaderStageSet.emplace_back(ShaderStage);
		}

		void setInputAssemblyState(const vk::PipelineInputAssemblyStateCreateInfo& vValue) { m_InputAssemblyStateCreateInfo = vValue; }
		void addVertexBinding(const vk::VertexInputBindingDescription& vValue) { m_VertexBindingDescriptionSet.emplace_back(vValue); }
		void addVertexAttribute(const vk::VertexInputAttributeDescription& vValue) { m_VertexAttributeDescriptionSet.emplace_back(vValue); }
		void setTopolopy(vk::PrimitiveTopology vValue) { m_InputAssemblyStateCreateInfo.topology = vValue; }
		void setPrimitiveRestartEnable(vk::Bool32 vValue) { m_InputAssemblyStateCreateInfo.primitiveRestartEnable = vValue; }

		void setTessellationState(const vk::PipelineTessellationStateCreateInfo& vValue) { m_TessellationStateCreateInfo = vValue; }

		void setViewport(const vk::Viewport& vValue) { m_Viewport = vValue; }
		void setViewportSize(float vWidth, float vHeight) { m_Viewport.width = vWidth, m_Viewport.height = vHeight; }
		void setScissor(const vk::Rect2D& vValue) { m_Scissor = vValue; }

		void setRasterizationState(const vk::PipelineRasterizationStateCreateInfo& vValue) { m_RasterizationStateCreateInfo = vValue; }
		void setDepthClampEnable(vk::Bool32 vValue) { m_RasterizationStateCreateInfo.depthClampEnable = vValue; }
		void setRasterizerDiscardEnable(vk::Bool32 vValue) { m_RasterizationStateCreateInfo.rasterizerDiscardEnable = vValue; }
		void setPolygonMode(vk::PolygonMode vValue) { m_RasterizationStateCreateInfo.polygonMode = vValue; }
		void setCullMode(vk::CullModeFlags vValue) { m_RasterizationStateCreateInfo.cullMode = vValue; }
		void setFrontFace(vk::FrontFace vValue) { m_RasterizationStateCreateInfo.frontFace = vValue; }
		void setDepthBiasEnable(vk::Bool32 vValue) { m_RasterizationStateCreateInfo.depthBiasEnable = vValue; }
		void setDepthBiasConstantFactor(float vValue) { m_RasterizationStateCreateInfo.depthBiasConstantFactor = vValue; }
		void setDepthBiasClamp(float vValue) { m_RasterizationStateCreateInfo.depthBiasClamp = vValue; }
		void setDepthBiasSlopeFactor(float vValue) { m_RasterizationStateCreateInfo.depthBiasSlopeFactor = vValue; }
		void setLineWidth(float vValue) { m_RasterizationStateCreateInfo.lineWidth = vValue; }

		void setMultisampleState(const vk::PipelineMultisampleStateCreateInfo& vValue) { m_MultisampleStateCreateInfo = vValue; }
		void setRasterizationSamples(vk::SampleCountFlagBits vValue) { m_MultisampleStateCreateInfo.rasterizationSamples = vValue; }
		void setSampleShadingEnable(vk::Bool32 vValue) { m_MultisampleStateCreateInfo.sampleShadingEnable = vValue; }
		void setMinSampleShading(float vValue) { m_MultisampleStateCreateInfo.minSampleShading = vValue; }
		void setPSampleMask(const vk::SampleMask* vValue) { m_MultisampleStateCreateInfo.pSampleMask = vValue; }
		void setAlphaToCoverageEnable(vk::Bool32 vValue) { m_MultisampleStateCreateInfo.alphaToCoverageEnable = vValue; }
		void setAlphaToOneEnable(vk::Bool32 vValue) { m_MultisampleStateCreateInfo.alphaToOneEnable = vValue; }

		void setDepthStencilState(const vk::PipelineDepthStencilStateCreateInfo& vValue) { m_DepthStencilStateCreateInfo = vValue; }
		void setDepthTestEnable(vk::Bool32 vValue) { m_DepthStencilStateCreateInfo.depthTestEnable = vValue; }
		void setDepthWriteEnable(vk::Bool32 vValue) { m_DepthStencilStateCreateInfo.depthWriteEnable = vValue; }
		void setDepthCompareOp(vk::CompareOp vValue) { m_DepthStencilStateCreateInfo.depthCompareOp = vValue; }
		void setDepthBoundsTestEnable(vk::Bool32 vValue) { m_DepthStencilStateCreateInfo.depthBoundsTestEnable = vValue; }
		void setStencilTestEnable(vk::Bool32 vValue) { m_DepthStencilStateCreateInfo.stencilTestEnable = vValue; }
		void setFront(vk::StencilOpState vValue) { m_DepthStencilStateCreateInfo.front = vValue; }
		void setBack(vk::StencilOpState vValue) { m_DepthStencilStateCreateInfo.back = vValue; }
		void setMinDepthBounds(float vValue) { m_DepthStencilStateCreateInfo.minDepthBounds = vValue; }
		void setMaxDepthBounds(float vValue) { m_DepthStencilStateCreateInfo.maxDepthBounds = vValue; }

		void addColorBlendAttachment(const vk::PipelineColorBlendAttachmentState& vColorBlendAttachmentState) { m_ColorBlendAttachmentStateSet.emplace_back(vColorBlendAttachmentState); }
		void setColorBlendState(const vk::PipelineColorBlendStateCreateInfo &vValue) { m_ColorBlendStateCreateInfo = vValue; }
		void setLogicOpEnable(vk::Bool32 vValue) { m_ColorBlendStateCreateInfo.logicOpEnable = vValue; }
		void setLogicOp(vk::LogicOp vValue) { m_ColorBlendStateCreateInfo.logicOp = vValue; }
		void setBlendConstants(float r, float g, float b, float a) { float *bc = m_ColorBlendStateCreateInfo.blendConstants; bc[0] = r; bc[1] = g; bc[2] = b; bc[3] = a; }

		void addDynamicState(const vk::DynamicState& vDynamicState) { m_DynamicStateSet.emplace_back(vDynamicState); }

		void setSubPass(uint32_t vSubPass) { m_Subpass = vSubPass; }

	private:
		vk::PipelineInputAssemblyStateCreateInfo				m_InputAssemblyStateCreateInfo;
		vk::PipelineVertexInputStateCreateInfo					m_VertexInputStateCreateInfo;
		std::optional<vk::PipelineTessellationStateCreateInfo>	m_TessellationStateCreateInfo;
		vk::Viewport											m_Viewport;
		vk::Rect2D												m_Scissor;
		vk::PipelineViewportStateCreateInfo						m_ViewportStateCreateInfo;
		vk::PipelineRasterizationStateCreateInfo				m_RasterizationStateCreateInfo;
		vk::PipelineMultisampleStateCreateInfo					m_MultisampleStateCreateInfo;
		vk::PipelineDepthStencilStateCreateInfo					m_DepthStencilStateCreateInfo;
		vk::PipelineColorBlendStateCreateInfo					m_ColorBlendStateCreateInfo;
		vk::PipelineDynamicStateCreateInfo						m_DynamicStateCreateInfo;

		std::vector<vk::PipelineShaderStageCreateInfo>			m_ShaderStageSet;
		std::vector<vk::PipelineColorBlendAttachmentState>		m_ColorBlendAttachmentStateSet;
		std::vector<vk::VertexInputAttributeDescription>		m_VertexAttributeDescriptionSet;
		std::vector<vk::VertexInputBindingDescription>			m_VertexBindingDescriptionSet;
		std::vector<vk::DynamicState>							m_DynamicStateSet;

		uint32_t m_Subpass = 0;

		vk::GraphicsPipelineCreateInfo m_PipelineCreateInfo;

		void __initPipeline(uint32_t vWidth, uint32_t vHeight)
		{
			m_InputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

			m_Viewport = vk::Viewport{ 0.0, 0.0, static_cast<float>(vWidth), static_cast<float>(vHeight), 0.0, 1.0 };
			m_Scissor = vk::Rect2D{ {0, 0}, {vWidth, vHeight} };

			m_RasterizationStateCreateInfo.lineWidth = 1.0f;
			m_RasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
			m_RasterizationStateCreateInfo.frontFace = vk::FrontFace::eClockwise;

			m_DepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
			m_DepthStencilStateCreateInfo.depthCompareOp = vk::CompareOp::eLessOrEqual;
			m_DepthStencilStateCreateInfo.minDepthBounds = 0.0f;
			m_DepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
			m_DepthStencilStateCreateInfo.back.compareOp = vk::CompareOp::eAlways;
			m_DepthStencilStateCreateInfo.front = m_DepthStencilStateCreateInfo.back;
		}

		void __preparePipelineCreateInfo(const vk::PipelineLayout& vPipelineLayout, const vk::RenderPass& vRenderPass)
		{
			m_VertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_VertexBindingDescriptionSet.size());
			m_VertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexAttributeDescriptionSet.size());
			m_VertexInputStateCreateInfo.pVertexBindingDescriptions = m_VertexBindingDescriptionSet.data();
			m_VertexInputStateCreateInfo.pVertexAttributeDescriptions = m_VertexAttributeDescriptionSet.data();

			m_ViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo{ {}, 1, &m_Viewport, 1, &m_Scissor };

			__addDefaulBlendIfNecessary();
			m_ColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(m_ColorBlendAttachmentStateSet.size());
			m_ColorBlendStateCreateInfo.pAttachments = m_ColorBlendAttachmentStateSet.data();

			m_DynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo{ {}, static_cast<uint32_t>(m_DynamicStateSet.size()), m_DynamicStateSet.data() };

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
			m_PipelineCreateInfo.subpass = m_Subpass;
		}

		void __addDefaulBlendIfNecessary()
		{
			if (m_ColorBlendAttachmentStateSet.empty())
			{
				vk::PipelineColorBlendAttachmentState ColorBlendAttachment;
				ColorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
				ColorBlendAttachment.blendEnable = VK_FALSE;
				ColorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
				ColorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
				ColorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
				ColorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
				ColorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
				ColorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;

				m_ColorBlendAttachmentStateSet.emplace_back(ColorBlendAttachment);
			}
		}
	};
}