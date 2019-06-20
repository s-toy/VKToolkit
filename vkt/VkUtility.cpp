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
void hiveVKT::createBuffer(vk::DeviceSize vBufferSize, vk::BufferUsageFlags vBufferUsage, 
	vk::MemoryPropertyFlags vMemoryProperty, 
	vk::Buffer& voBuffer, vk::DeviceMemory& voBufferDeviceMemory)
{
	_ASSERT(CVkContext::getInstance()->isContextCreated());
	auto Device = CVkContext::getInstance()->getVulkanDevice();

	vk::BufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.size = vBufferSize;
	BufferCreateInfo.usage = vBufferUsage;
	BufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	voBuffer = Device.createBuffer(BufferCreateInfo);

	vk::MemoryRequirements MemoryRequirements = Device.getBufferMemoryRequirements(voBuffer);

	vk::MemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(MemoryRequirements.memoryTypeBits, vMemoryProperty);

	voBufferDeviceMemory = Device.allocateMemory(MemoryAllocateInfo);

	Device.bindBufferMemory(voBuffer, voBufferDeviceMemory, 0);
}

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::splitByStr(const std::string& vInput, const std::string& vDelimiter, std::vector<std::string>& voOutput)
{
	std::vector<std::string> SplitResult;

	if ("" == vInput)
	{
		return false;
	}

	std::string StringToSplit = vInput + vDelimiter;	//����ָ��ַ��������ȡ���һ��

	size_t DelimiterSize	= vDelimiter.size();
	size_t NextDelimiterPos	= StringToSplit.find(vDelimiter);
	size_t CurrentSize		= StringToSplit.size();

	while (NextDelimiterPos != std::string::npos)
	{
		std::string SubString = StringToSplit.substr(0, NextDelimiterPos);
		SplitResult.push_back(SubString);
		StringToSplit = StringToSplit.substr(NextDelimiterPos + DelimiterSize , CurrentSize);
		NextDelimiterPos = StringToSplit.find(vDelimiter);
		CurrentSize = StringToSplit.size();
	}
	
	voOutput = SplitResult;

	if (voOutput.size() != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
