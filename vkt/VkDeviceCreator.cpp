#include "VkDeviceCreator.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::Device hiveVKT::CVkDeviceCreator::create(const vk::PhysicalDevice& vPhysicalDevice)
{
	__prepareDeviceCreateInfo(vPhysicalDevice);
	return vPhysicalDevice.createDevice(m_DeviceCreateInfo, nullptr);
}

//***********************************************************************************************
//FUNCTION:
vk::UniqueDevice hiveVKT::CVkDeviceCreator::createUnique(const vk::PhysicalDevice& vPhysicalDevice)
{
	__prepareDeviceCreateInfo(vPhysicalDevice);
	return vPhysicalDevice.createDeviceUnique(m_DeviceCreateInfo, nullptr);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkDeviceCreator::addQueue(uint32_t vQueueFamilyIndex, uint32_t vQueueCount /*= 1*/, float vPriority /*= 0.0f*/)
{
	m_QueuePrioritySet.emplace_back(vQueueCount, vPriority);
	m_DeviceQueueCreateInfoSet.emplace_back(vk::DeviceQueueCreateFlags{}, vQueueFamilyIndex, vQueueCount, m_QueuePrioritySet.back().data());
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkDeviceCreator::__prepareDeviceCreateInfo(const vk::PhysicalDevice& vPhysicalDevice)
{
	__addDebugLayersAndExtensionsIfNecessary();

#ifdef _ENABLE_DEBUG_UTILS
	if (!__checkDeviceLayersAndExtensionsSupport(vPhysicalDevice))
		throw std::runtime_error("Not all requested layers and extensions are available.");
#endif

	m_DeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_DeviceLayerSet.size());
	m_DeviceCreateInfo.ppEnabledLayerNames = m_DeviceLayerSet.data();
	m_DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensionSet.size());
	m_DeviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensionSet.data();
	m_DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_DeviceQueueCreateInfoSet.size());
	m_DeviceCreateInfo.pQueueCreateInfos = m_DeviceQueueCreateInfoSet.data();
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkDeviceCreator::__addDebugLayersAndExtensionsIfNecessary()
{
#ifdef _ENABLE_DEBUG_UTILS
	m_DeviceLayerSet.emplace_back("VK_LAYER_LUNARG_standard_validation");
#endif
}

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::CVkDeviceCreator::__checkDeviceLayersAndExtensionsSupport(const vk::PhysicalDevice& vPhysicalDevice)
{
	auto DeviceLayerPropertiesSet = vPhysicalDevice.enumerateDeviceLayerProperties();
	auto DeviceExtensionPropertiesSet = vPhysicalDevice.enumerateDeviceExtensionProperties();

	std::set<std::string> RequiredLayerSet(m_DeviceLayerSet.begin(), m_DeviceLayerSet.end());
	std::set<std::string> RequiredExtensionSet(m_DeviceExtensionSet.begin(), m_DeviceExtensionSet.end());
	for (const auto& LayerProperty : DeviceLayerPropertiesSet) RequiredLayerSet.erase(LayerProperty.layerName);
	for (const auto& ExtensionProperty : DeviceExtensionPropertiesSet) RequiredExtensionSet.erase(ExtensionProperty.extensionName);

	return RequiredLayerSet.empty() && RequiredExtensionSet.empty();
}