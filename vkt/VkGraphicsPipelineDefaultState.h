#pragma once

namespace hiveVKT
{
	#define DefaultPipelineInputAssemblyStateCreateInfo	\
	{											        \
		vk::PipelineInputAssemblyStateCreateFlags(),    \
		vk::PrimitiveTopology::eTriangleList,           \
		false /*primitiveRestartEnable*/			    \
	}	

	#define DefaultPipelineVertexInputStateCreateInfo  \
	{                                                  \
		vk::PipelineVertexInputStateCreateFlags(),     \
		0,       /*vertexBindingDescriptionCount*/     \
		nullptr, /*pVertexInputBindingDescription*/    \
		0,       /*vertexAttributeDescriptionCount*/   \
		nullptr  /*pVertexAttributeDescriptions*/      \
	}

	#define DefaultPipelineViewportStateCreateInfo    \
	{												  \
		vk::PipelineViewportStateCreateFlags(),       \
		0,       /*viewportCount*/	/*0->1*/		  \
		nullptr, /*pViewports*/						  \
		0,       /*scissorCount*/	/*0->1*/		  \
		nullptr  /*pScissors*/						  \
	}

	#define DefaultPipelineRasterizationStateCreateInfo \
	{													\
		vk::PipelineRasterizationStateCreateFlags(),	\
		false, /*depthClampEnable*/						\
		false, /*rasterizerDiscardEnable*/				\
		vk::PolygonMode::eFill,							\
		vk::CullModeFlagBits::eBack,					\
		vk::FrontFace::eCounterClockwise,				\
		false, /*depthBiasEnable*/						\
		0.0,   /*depthBiasConstantFactor*/				\
		0.0,   /*depthBiasClamp*/						\
		0.0,   /*depthBiasSlopeFactor*/					\
		1.0    /*lineWidth*/							\
	}

	#define DefaultPipelineMultisampleStateCreateInfo   \
	{													\
		vk::PipelineMultisampleStateCreateFlags(),		\
		vk::SampleCountFlagBits::e1,					\
		false,   /*sampleShadingEnable*/				\
		0.0,     /*minSampleShading*/					\
		nullptr, /*pSampleMask*/						\
		false,   /*alphaToCoverageEnable*/				\
		false    /*alphaToOneEnable*/					\
	}

	#define DefaultPipelineDepthStencilStateCreateInfo  \
	{													\
		vk::PipelineDepthStencilStateCreateFlags(),		\
		false, /*depthTestEnable*/						\
		false, /*depthWriteEnable*/						\
		vk::CompareOp::eNever,							\
		false, /*depthBoundsTestEnable*/				\
		false, /*stencilTestEnable*/					\
		vk::StencilOpState(), /*front StencilOpState*/  \
		vk::StencilOpState(), /*back StencilOpState*/   \
		0.0, /*minDepthBounds*/						    \
		1.0  /*maxDepthBounds*/							\
	}

	#define DefaultPipelineColorBlendStateCreateInfo    \
	{                                                   \
		vk::PipelineColorBlendStateCreateFlags(),       \
		false,				 /*logicOpEnable*/          \
		vk::LogicOp::eClear,                            \
		0,					 /*attachmentCount*/        \
		nullptr,			 /*pAttachments*/           \
		{ { 0, 0, 0, 0 } }   /*blendConstants*/         \
	}                                                   

	#define DefaultPipelineDynamicStateCreateInfo       \
	{                                                   \
		vk::PipelineDynamicStateCreateFlags(),          \
		0,      /*dynamicStateCount*/                   \
		nullptr /*pDynamicStates*/                      \
	}

	#define DefaultPipelineColorBlendAttachmentState    \
	{												    \
		false,                  /*blendEnable*/         \
		vk::BlendFactor::eOne,  /*srcColorBlendFactor*/ \
		vk::BlendFactor::eZero, /*dstColorBlendFactor*/ \
		vk::BlendOp::eAdd,      /*colorBlendOp*/        \
		vk::BlendFactor::eOne,  /*srcAlphaBlendFactor*/ \
		vk::BlendFactor::eZero, /*dstAlphaBlendFactor*/ \
		vk::BlendOp::eAdd,      /*alphaBlendOp*/        \
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA /*colorWriteMask*/ \
	}
}
