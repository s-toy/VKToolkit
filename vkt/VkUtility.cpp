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
void hiveVKT::executeImmediately(const std::function<void(vk::CommandBuffer vCommandBuffer)>& vFunction)
{
	_ASSERT(CVkContext::getInstance()->isContextCreated());
	auto Device = CVkContext::getInstance()->getVulkanDevice();
	auto CommandPool = CVkContext::getInstance()->getComprehensiveCommandPool();
	auto Queue = CVkContext::getInstance()->getComprehensiveQueue();

	vk::CommandBufferAllocateInfo CommandBufferAllocateInfo;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	auto CommandBuffers = Device.allocateCommandBuffers(CommandBufferAllocateInfo);

	vk::CommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	CommandBuffers[0].begin(CommandBufferBeginInfo);
	vFunction(CommandBuffers[0]);
	CommandBuffers[0].end();

	vk::SubmitInfo SubmitInfo;
	SubmitInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());
	SubmitInfo.pCommandBuffers = CommandBuffers.data();

	Queue.submit(SubmitInfo, vk::Fence{});

	Device.waitIdle();//NOTE

	Device.freeCommandBuffers(CommandPool, CommandBuffers);
}

//***********************************************************************************************
//FUNCTION:
vk::Result hiveVKT::createBuffer(vk::DeviceSize vBufferSize, vk::BufferUsageFlags vBufferUsage, 
	vk::MemoryPropertyFlags vMemoryProperty, 
	vk::Buffer& voBuffer, vk::DeviceMemory& voBufferDeviceMemory)
{
	_ASSERT(CVkContext::getInstance()->isContextCreated());
	auto Device = CVkContext::getInstance()->getVulkanDevice();

	vk::Result Result;

	vk::BufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.size = vBufferSize;
	BufferCreateInfo.usage = vBufferUsage;
	BufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	Result = Device.createBuffer(&BufferCreateInfo, nullptr, &voBuffer);
	if (Result != vk::Result::eSuccess)
	{
		return Result;
	}

	vk::MemoryRequirements MemoryRequirements = Device.getBufferMemoryRequirements(voBuffer);

	vk::MemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(MemoryRequirements.memoryTypeBits, vMemoryProperty);

	Result = Device.allocateMemory(&MemoryAllocateInfo, nullptr, &voBufferDeviceMemory);
	if (Result != vk::Result::eSuccess)
	{
		return Result;
	}

	Device.bindBufferMemory(voBuffer, voBufferDeviceMemory, 0);//TODO: Get result and check

	return vk::Result::eSuccess;
}