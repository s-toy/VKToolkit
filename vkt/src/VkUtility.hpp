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
}