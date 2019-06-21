#pragma once
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	namespace objectCreator
	{
		vk::Pipeline createGraphicsPipeline(vk::Device vDevice, const vk::GraphicsPipelineCreateInfo& vCreateInfo);
	}
}