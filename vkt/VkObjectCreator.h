#pragma once
#include <vulkan/vulkan.hpp>
#include "Common.h"
#include "VkGraphicsPipelineDescriptor.h"

namespace hiveVKT
{
	class CVkObjectCreator
	{
	public:
		Singleton(CVkObjectCreator);
		vk::Pipeline createGraphicsPipeline(vk::Device vDevice, const CVkGraphicsPipelineDescriptor* vDescriptor);

	private:
		CVkObjectCreator() = default;
	};
}