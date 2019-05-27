#pragma once
#include <set>
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	class CVkDeviceCreator
	{
	public:
		vk::Device create(const vk::PhysicalDevice& vPhysicalDevice);
		vk::UniqueDevice createUnique(const vk::PhysicalDevice& vPhysicalDevice);

		void addLayer(const char* vLayer) { m_DeviceLayerSet.emplace_back(vLayer); }
		void addExtension(const char* vExtension) { m_DeviceExtensionSet.emplace_back(vExtension); }
		void setEnabledLayers(const std::vector<const char*> vEnabledLayers) { m_DeviceLayerSet = vEnabledLayers; }
		void setEnabledExtensions(const std::vector<const char*> vEnabledExtensions) { m_DeviceExtensionSet = vEnabledExtensions; }
		void addQueue(uint32_t vQueueFamilyIndex, uint32_t vQueueCount = 1, float vPriority = 0.0f);

		void setPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures* vValue) { m_DeviceCreateInfo.pEnabledFeatures = vValue; }

	private:
		vk::DeviceCreateInfo					m_DeviceCreateInfo;
		std::vector<const char *>				m_DeviceLayerSet;
		std::vector<const char *>				m_DeviceExtensionSet;
		std::vector<vk::DeviceQueueCreateInfo>	m_DeviceQueueCreateInfoSet;
		std::vector<std::vector<float>>			m_QueuePrioritySet;

		void __prepareDeviceCreateInfo(const vk::PhysicalDevice& vPhysicalDevice);
		void __addDebugLayersAndExtensionsIfNecessary();
		bool __checkDeviceLayersAndExtensionsSupport(const vk::PhysicalDevice& vPhysicalDevice);
	};
}