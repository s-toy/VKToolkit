#pragma once
#include <set>
#include <functional>
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	bool checkInstanceLayersSupport(const std::vector<std::string>& vRequiredInstanceLayerSet);

	uint32_t findMemoryTypeIndex(uint32_t vMemoryTypeFilter, vk::MemoryPropertyFlags vMemoryPropertyFilter);

	void executeImmediately(const std::function<void(vk::CommandBuffer vCommandBuffer)>& vFunction);

	void createBuffer(vk::DeviceSize vBufferSize, vk::BufferUsageFlags vBufferUsage, vk::MemoryPropertyFlags vMemoryProperty, vk::Buffer& voBuffer, vk::DeviceMemory& voBufferDeviceMemory);
}