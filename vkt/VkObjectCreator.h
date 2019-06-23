#pragma once
#include <vulkan/vulkan.hpp>
#include <common/Singleton.h>

namespace hiveVKT
{
	class CVkInstanceDescriptor;
	class CVkDeviceDescriptor;
	class CVkGraphicsPipelineDescriptor;

	class CVkObjectCreator : public hiveDesignPattern::CSingleton<CVkObjectCreator>
	{
	public:
		vk::Instance	createInstance(CVkInstanceDescriptor& vDescriptor);
		vk::Device		createDevice(vk::PhysicalDevice vPhysicalDevice, CVkDeviceDescriptor& vDescriptor);
		vk::Pipeline	createGraphicsPipeline(vk::Device vDevice, CVkGraphicsPipelineDescriptor& vDescriptor);

		friend class hiveDesignPattern::CSingleton<CVkObjectCreator>;
	};
}