#include "VkObjectCreator.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::Pipeline objectCreator::createGraphicsPipeline(vk::Device vDevice, const vk::GraphicsPipelineCreateInfo& vCreateInfo)
{
	_ASSERTE(vDevice);

	//TODO: 有效性检查

	return vDevice.createGraphicsPipeline(nullptr, vCreateInfo);
}