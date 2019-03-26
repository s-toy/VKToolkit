#pragma once
#include <set>
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	static bool checkInstanceLayersSupport(const std::vector<std::string>& vRequiredInstanceLayerSet)
	{
		auto InstanceLayerPropertiesSet = vk::enumerateInstanceLayerProperties();

		std::set<std::string> RequiredInstanceLayerSet(vRequiredInstanceLayerSet.begin(), vRequiredInstanceLayerSet.end());

		for (const auto& LayerProperty : InstanceLayerPropertiesSet) RequiredInstanceLayerSet.erase(LayerProperty.layerName);

		return RequiredInstanceLayerSet.empty();
	}

	static uint32_t findMemoryTypeIndex(const vk::PhysicalDeviceMemoryProperties vPhysicalDeviceMemoryProperties, uint32_t vMemoryTypeFilter, vk::MemoryPropertyFlags vMemoryPropertyFilter)
	{
		for (uint32_t i = 0; i < vPhysicalDeviceMemoryProperties.memoryTypeCount; ++i, vMemoryTypeFilter >>= 1)
		{
			if (vMemoryTypeFilter & 1)
			{
				if ((vPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & vMemoryPropertyFilter) == vMemoryPropertyFilter)
					return i;
			}
		}

		return VK_MAX_MEMORY_TYPES + 1;
	}
}