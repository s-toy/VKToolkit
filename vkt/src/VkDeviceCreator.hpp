#pragma once
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	class CVkDeviceCreator
	{
	public:
		vk::Device create(const vk::PhysicalDevice& vPhysicalDevice)
		{
			__prepareDeviceCreateInfo();
			return vPhysicalDevice.createDevice(m_DeviceCreateInfo, nullptr);
		}

		vk::UniqueDevice createUnique(const vk::PhysicalDevice& vPhysicalDevice)
		{
			__prepareDeviceCreateInfo();
			return vPhysicalDevice.createDeviceUnique(m_DeviceCreateInfo, nullptr);
		}

		void addLayer(const char* vLayer) { m_DeviceLayerSet.emplace_back(vLayer); }
		void addExtension(const char* vExtension) { m_DeviceExtensionSet.emplace_back(vExtension); }
		void addQueue(uint32_t vQueueFamilyIndex, uint32_t vQueueCount = 1, float vPriority = 0.0f)
		{
			m_QueuePrioritieSet.emplace_back(vQueueCount, vPriority);
			m_DeviceQueueCreateInfoSet.emplace_back(vk::DeviceQueueCreateFlags{}, vQueueFamilyIndex, vQueueCount, m_QueuePrioritieSet.back().data());
		}

		void setPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures* vValue) { m_DeviceCreateInfo.pEnabledFeatures = vValue; }

		void defaultLayersAndExtensions(bool vEnable) { m_EnableDefaultLayersAndExtensions = vEnable; }

	private:
		vk::DeviceCreateInfo					m_DeviceCreateInfo;
		std::vector<const char *>				m_DeviceLayerSet;
		std::vector<const char *>				m_DeviceExtensionSet;
		std::vector<vk::DeviceQueueCreateInfo>	m_DeviceQueueCreateInfoSet;
		std::vector<std::vector<float>>			m_QueuePrioritieSet;

		bool m_EnableDefaultLayersAndExtensions = true;

		void __prepareDeviceCreateInfo()
		{
			__addDefaultLayersAndExtensionsIfNecessary();

			m_DeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_DeviceLayerSet.size());
			m_DeviceCreateInfo.ppEnabledLayerNames = m_DeviceLayerSet.data();
			m_DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensionSet.size());
			m_DeviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensionSet.data();
			m_DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_DeviceQueueCreateInfoSet.size());
			m_DeviceCreateInfo.pQueueCreateInfos = m_DeviceQueueCreateInfoSet.data();
		}

		void __addDefaultLayersAndExtensionsIfNecessary()
		{
			if (!m_EnableDefaultLayersAndExtensions) return;

		#ifdef _ENABLE_DEBUG_UTILS
			m_DeviceLayerSet.emplace_back("VK_LAYER_LUNARG_standard_validation");
		#endif

			m_DeviceExtensionSet.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}
	};
}