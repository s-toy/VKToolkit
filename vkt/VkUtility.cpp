#include "VkUtility.h"
#include "VkContext.h"

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::checkInstanceLayersSupport(const std::vector<std::string>& vRequiredInstanceLayerSet)
{
	auto InstanceLayerPropertiesSet = vk::enumerateInstanceLayerProperties();

	std::set<std::string> RequiredInstanceLayerSet(vRequiredInstanceLayerSet.begin(), vRequiredInstanceLayerSet.end());

	for (const auto& LayerProperty : InstanceLayerPropertiesSet) RequiredInstanceLayerSet.erase(LayerProperty.layerName);

	return RequiredInstanceLayerSet.empty();
}

//***********************************************************************************************
//FUNCTION:
uint32_t hiveVKT::findMemoryTypeIndex(uint32_t vMemoryTypeFilter, vk::MemoryPropertyFlags vMemoryPropertyFilter)
{
	vk::PhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties = CVkContext::getInstance()->getPhysicalDevice().getMemoryProperties();

	for (uint32_t i = 0; i < PhysicalDeviceMemoryProperties.memoryTypeCount; ++i, vMemoryTypeFilter >>= 1)
	{
		if (vMemoryTypeFilter & 1)
		{
			if ((PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & vMemoryPropertyFilter) == vMemoryPropertyFilter)
				return i;
		}
	}

	return VK_MAX_MEMORY_TYPES + 1;
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::executeImmediately(vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue, const std::function<void(vk::CommandBuffer vCommandBuffer)>& vFunction)
{
	vk::CommandBufferAllocateInfo CommandBufferAllocateInfo;
	CommandBufferAllocateInfo.commandPool = vCommandPool;
	CommandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	auto CommandBuffers = vDevice.allocateCommandBuffers(CommandBufferAllocateInfo);

	vk::CommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	CommandBuffers[0].begin(CommandBufferBeginInfo);
	vFunction(CommandBuffers[0]);
	CommandBuffers[0].end();

	vk::SubmitInfo SubmitInfo;
	SubmitInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());
	SubmitInfo.pCommandBuffers = CommandBuffers.data();

	vQueue.submit(SubmitInfo, vk::Fence{});

	vDevice.waitIdle();//NOTE

	vDevice.freeCommandBuffers(vCommandPool, CommandBuffers);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::createBuffer(vk::Device vDevice, vk::DeviceSize vBufferSize, vk::BufferUsageFlags vBufferUsage, vk::MemoryPropertyFlags vMemoryProperty, vk::Buffer& voBuffer, vk::DeviceMemory& voBufferDeviceMemory)
{
	vk::BufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.size = vBufferSize;
	BufferCreateInfo.usage = vBufferUsage;
	BufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	voBuffer = vDevice.createBuffer(BufferCreateInfo);

	vk::MemoryRequirements MemoryRequirements = vDevice.getBufferMemoryRequirements(voBuffer);

	vk::MemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(MemoryRequirements.memoryTypeBits, vMemoryProperty);

	voBufferDeviceMemory = vDevice.allocateMemory(MemoryAllocateInfo);

	vDevice.bindBufferMemory(voBuffer, voBufferDeviceMemory, 0);
}
