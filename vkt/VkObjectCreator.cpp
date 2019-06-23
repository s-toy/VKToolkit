#include "VkObjectCreator.h"
#include <common/CommonMicro.h>
#include "VkInstanceDescriptor.h"
#include "VkDeviceDescriptor.h"
#include "VkGraphicsPipelineDescriptor.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::Instance CVkObjectCreator::createInstance(CVkInstanceDescriptor& vDescriptor)
{
	_HIVE_EARLY_RETURN(!vDescriptor._isValid(), "Fail to create instance because the descriptor is invalid", nullptr);

	return vk::createInstance(vDescriptor._getInstanceCreateInfo());
}

vk::Device hiveVKT::CVkObjectCreator::createDevice(vk::PhysicalDevice vPhysicalDevice, CVkDeviceDescriptor& vDescriptor)
{
	_ASSERTE(vPhysicalDevice);
	_HIVE_EARLY_RETURN(!vDescriptor._isValid(), "Fail to create device because the descriptor is invalid", nullptr);

	return vPhysicalDevice.createDevice(vDescriptor._getDeviceCreateInfo());
}

//***********************************************************************************************
//FUNCTION:
vk::Pipeline CVkObjectCreator::createGraphicsPipeline(vk::Device vDevice, CVkGraphicsPipelineDescriptor& vDescriptor)
{
	_ASSERTE(vDevice);
	_HIVE_EARLY_RETURN(!vDescriptor._isValid(), "Fail to create graphics pipeline because the descriptor is invalid", nullptr);

	return vDevice.createGraphicsPipeline(nullptr, vDescriptor._getPipelineCreateInfo());
}