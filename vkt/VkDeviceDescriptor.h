#pragma once
#include <set>
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	class CVkDeviceDescriptor
	{
	public:
		void addLayer(const char* vLayer) { m_DeviceLayerSet.emplace_back(vLayer); }
		void addExtension(const char* vExtension) { m_DeviceExtensionSet.emplace_back(vExtension); }
		void addQueue(uint32_t vQueueFamilyIndex, uint32_t vQueueCount = 1, float vPriority = 0.0f);

		void setPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures* vValue) { m_DeviceCreateInfo.pEnabledFeatures = vValue; }

	protected:
		bool _isValid() const { return true; } //TODO:检查有效性
		const vk::DeviceCreateInfo& _getDeviceCreateInfo() { __assemblingDeviceCreateInfo(); return m_DeviceCreateInfo; }

	private:
		vk::DeviceCreateInfo					m_DeviceCreateInfo;
		std::vector<const char *>				m_DeviceLayerSet;
		std::vector<const char *>				m_DeviceExtensionSet;
		std::vector<vk::DeviceQueueCreateInfo>	m_DeviceQueueCreateInfoSet;
		std::vector<std::vector<float>>			m_QueuePrioritySet;

		void __assemblingDeviceCreateInfo();

		friend class CVkObjectCreator;
	};
}