#include "VkPhysicalDeviceInfoHelper.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkPhysicalDeviceInfoHelper::init(vk::PhysicalDevice vPhysicalDevice)
{
	m_pPhysicalDevice = vPhysicalDevice;
}