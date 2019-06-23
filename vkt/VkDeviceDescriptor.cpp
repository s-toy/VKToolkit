#include "VkDeviceDescriptor.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkDeviceDescriptor::addQueue(uint32_t vQueueFamilyIndex, uint32_t vQueueCount /*= 1*/, float vPriority /*= 0.0f*/)
{
	m_QueuePrioritySet.emplace_back(vQueueCount, vPriority);
	m_DeviceQueueCreateInfoSet.emplace_back(vk::DeviceQueueCreateFlags{}, vQueueFamilyIndex, vQueueCount, m_QueuePrioritySet.back().data());
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkDeviceDescriptor::__assemblingDeviceCreateInfo()
{
	m_DeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_DeviceLayerSet.size());
	m_DeviceCreateInfo.ppEnabledLayerNames = m_DeviceLayerSet.data();
	m_DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensionSet.size());
	m_DeviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensionSet.data();
	m_DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_DeviceQueueCreateInfoSet.size());
	m_DeviceCreateInfo.pQueueCreateInfos = m_DeviceQueueCreateInfoSet.data();
}