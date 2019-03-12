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
			return vPhysicalDevice.createDevice(m_DeivceCreateInfo, nullptr);
		}

		vk::UniqueDevice createUnique(const vk::PhysicalDevice& vPhysicalDevice)
		{
			__prepareDeviceCreateInfo();
			return vPhysicalDevice.createDeviceUnique(m_DeivceCreateInfo, nullptr);
		}

		void addLayer(const char* vLayer) { m_DeviceLayerSet.emplace_back(vLayer); }
		void addExtension(const char* vExtension) { m_DeviceExtensionSet.emplace_back(vExtension); }
		void addQueue(uint32_t vQueueFamilyIndex, uint32_t vQueueCount = 1, float vPriority = 0.0f)
		{
			m_QueuePrioritieSet.emplace_back(vQueueCount, vPriority);
			m_DeviceQueueCreateInfoSet.emplace_back(vk::DeviceQueueCreateFlags{}, vQueueFamilyIndex, vQueueCount, m_QueuePrioritieSet.back().data());
		}

		void setPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures* vValue) { m_DeivceCreateInfo.pEnabledFeatures = vValue; }

		void defaultLayersAndExtensions(bool vEnable) { m_EnableDefaultLayersAndExtensions = vEnable; }

	private:
		vk::DeviceCreateInfo					m_DeivceCreateInfo;
		std::vector<const char *>				m_DeviceLayerSet;
		std::vector<const char *>				m_DeviceExtensionSet;
		std::vector<vk::DeviceQueueCreateInfo>	m_DeviceQueueCreateInfoSet;
		std::vector<std::vector<float>>			m_QueuePrioritieSet;

		bool m_EnableDefaultLayersAndExtensions = true;

		void __prepareDeviceCreateInfo()
		{
			__addDefaultLayersAndExtensionsIfNecessary();

			m_DeivceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_DeviceLayerSet.size());
			m_DeivceCreateInfo.ppEnabledLayerNames = m_DeviceLayerSet.data();
			m_DeivceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensionSet.size());
			m_DeivceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensionSet.data();
			m_DeivceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_DeviceQueueCreateInfoSet.size());
			m_DeivceCreateInfo.pQueueCreateInfos = m_DeviceQueueCreateInfoSet.data();
		}

		void __addDefaultLayersAndExtensionsIfNecessary()
		{
			if (!m_EnableDefaultLayersAndExtensions) return;

		#ifdef _ENABLE_VK_DEBUG_UTILS
			m_DeviceLayerSet.emplace_back("VK_LAYER_LUNARG_standard_validation");
		#endif

			m_DeviceExtensionSet.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}
	};
}