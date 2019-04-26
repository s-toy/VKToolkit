#pragma once
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	class CVkDeviceCreator
	{
	public:
		vk::Device create(const vk::PhysicalDevice& vPhysicalDevice)
		{
			__prepareDeviceCreateInfo(vPhysicalDevice);
			return vPhysicalDevice.createDevice(m_DeviceCreateInfo, nullptr);
		}

		vk::UniqueDevice createUnique(const vk::PhysicalDevice& vPhysicalDevice)
		{
			__prepareDeviceCreateInfo(vPhysicalDevice);
			return vPhysicalDevice.createDeviceUnique(m_DeviceCreateInfo, nullptr);
		}

		void addLayer(const char* vLayer) { m_DeviceLayerSet.emplace_back(vLayer); }
		void addExtension(const char* vExtension) { m_DeviceExtensionSet.emplace_back(vExtension); }
		void setEnabledLayers(const const std::vector<const char*> vEnabledLayers) { m_DeviceLayerSet = vEnabledLayers; }
		void setEnabledExtensions(const std::vector<const char*> vEnabledExtensions) { m_DeviceExtensionSet = vEnabledExtensions; }

		void addQueue(uint32_t vQueueFamilyIndex, uint32_t vQueueCount = 1, float vPriority = 0.0f)
		{
			m_QueuePrioritieSet.emplace_back(vQueueCount, vPriority);
			m_DeviceQueueCreateInfoSet.emplace_back(vk::DeviceQueueCreateFlags{}, vQueueFamilyIndex, vQueueCount, m_QueuePrioritieSet.back().data());
		}

		void setPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures* vValue) { m_DeviceCreateInfo.pEnabledFeatures = vValue; }

	private:
		vk::DeviceCreateInfo					m_DeviceCreateInfo;
		std::vector<const char *>				m_DeviceLayerSet;
		std::vector<const char *>				m_DeviceExtensionSet;
		std::vector<vk::DeviceQueueCreateInfo>	m_DeviceQueueCreateInfoSet;
		std::vector<std::vector<float>>			m_QueuePrioritieSet;

		void __prepareDeviceCreateInfo(const vk::PhysicalDevice& vPhysicalDevice)
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

		bool __checkDeviceLayersAndExtensionsSupport(const vk::PhysicalDevice& vPhysicalDevice)
		{
			auto DeviceLayerPropertiesSet = vPhysicalDevice.enumerateDeviceLayerProperties();
			auto DeviceExtensionPropertiesSet = vPhysicalDevice.enumerateDeviceExtensionProperties();

			std::set<std::string> RequiredLayerSet(m_DeviceLayerSet.begin(), m_DeviceLayerSet.end());
			std::set<std::string> RequiredExtensionSet(m_DeviceExtensionSet.begin(), m_DeviceExtensionSet.end());
			for (const auto& LayerProperty : DeviceLayerPropertiesSet) RequiredLayerSet.erase(LayerProperty.layerName);
			for (const auto& ExtensionProperty : DeviceExtensionPropertiesSet) RequiredExtensionSet.erase(ExtensionProperty.extensionName);

			return RequiredLayerSet.empty() && RequiredExtensionSet.empty();
		}

		void __addDebugLayersAndExtensionsIfNecessary()
		{
#ifdef _ENABLE_DEBUG_UTILS
			m_DeviceLayerSet.emplace_back("VK_LAYER_LUNARG_standard_validation");
#endif
		}
	};
}