#include "VkObjectCreator.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::Pipeline objectCreator::createGraphicsPipeline(vk::Device vDevice, const vk::GraphicsPipelineCreateInfo& vCreateInfo)
{
	_ASSERTE(vDevice);

	//TODO: ��Ч�Լ��

	return vDevice.createGraphicsPipeline(nullptr, vCreateInfo);
}