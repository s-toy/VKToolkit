#pragma once
#include <set>
#include <functional>
#include <vulkan/vulkan.hpp>
#include <string>
#include "Export.h"

namespace hiveVKT
{
	VKT_DECLSPEC bool checkInstanceLayersSupport(const std::vector<std::string>& vRequiredInstanceLayerSet);

	VKT_DECLSPEC uint32_t findMemoryTypeIndex(uint32_t vMemoryTypeFilter, vk::MemoryPropertyFlags vMemoryPropertyFilter);

	VKT_DECLSPEC void executeImmediately(const std::function<void(vk::CommandBuffer vCommandBuffer)>& vFunction);

	VKT_DECLSPEC vk::Result createBuffer(vk::DeviceSize vBufferSize, vk::BufferUsageFlags vBufferUsage, vk::MemoryPropertyFlags vMemoryProperty, vk::Buffer& voBuffer, vk::DeviceMemory& voBufferDeviceMemory);
}