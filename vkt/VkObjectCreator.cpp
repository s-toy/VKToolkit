#include "VkObjectCreator.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::Pipeline hiveVKT::CVkObjectCreator::createGraphicsPipeline(vk::Device vDevice, const CVkGraphicsPipelineDescriptor* vDescriptor)
{
	_ASSERTE(vDescriptor != nullptr);

	if (!vDescriptor->isValid())
	{
		//TODO: output warning
		return nullptr;
	}

	return vDevice.createGraphicsPipeline(nullptr, vDescriptor->getPipelineCreateInfo());
}