#include "VkGenericImage.h"
#include "VkContext.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGenericImage::create(const vk::ImageCreateInfo& vImageCreateInfo, vk::ImageViewType vImageViewType, vk::ImageAspectFlags vImageAspectMasks, bool vHostVisible)
{
	if (m_IsImageCreated)
		return;

	_ASSERT(vImageCreateInfo.arrayLayers == 1);
	_ASSERT(CVkContext::getInstance()->isContextCreated());
	auto Device = CVkContext::getInstance()->getVulkanDevice();

	m_CurrentImageLayoutSet.resize(vImageCreateInfo.mipLevels, vImageCreateInfo.initialLayout);
	m_ImageCreateInfo = vImageCreateInfo;
	m_pImage = Device.createImage(vImageCreateInfo);

	auto MemoryRequirements = Device.getImageMemoryRequirements(m_pImage);

	vk::MemoryPropertyFlags MemoryProperties = {};
	if (vHostVisible)
		MemoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

	auto MemoryTypeIndex = findMemoryTypeIndex(MemoryRequirements.memoryTypeBits, MemoryProperties);
	_ASSERT(MemoryTypeIndex != VK_MAX_MEMORY_TYPES + 1);

	vk::MemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = MemoryTypeIndex;

	m_pDeviceMemory = Device.allocateMemory(MemoryAllocateInfo);

	Device.bindImageMemory(m_pImage, m_pDeviceMemory, 0);

	vk::ImageViewCreateInfo ImageViewCreateInfo = {};
	ImageViewCreateInfo.image = m_pImage;
	ImageViewCreateInfo.viewType = vImageViewType;
	ImageViewCreateInfo.format = vImageCreateInfo.format;
	ImageViewCreateInfo.components = { vk::ComponentSwizzle::eR,vk::ComponentSwizzle::eG,vk::ComponentSwizzle::eB,vk::ComponentSwizzle::eA };
	ImageViewCreateInfo.subresourceRange = vk::ImageSubresourceRange{ vImageAspectMasks,0,vImageCreateInfo.mipLevels,0,vImageCreateInfo.arrayLayers };

	m_pImageView = Device.createImageView(ImageViewCreateInfo);

	m_IsImageCreated = true;
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGenericImage::translateImageLayoutAtParticularMipmapLevel(vk::CommandBuffer vCommandBuffer, vk::ImageLayout vNewImageLayout, vk::ImageAspectFlags vImageAspectFlags, uint32_t vMipmapLevel)
{
	_ASSERT(vCommandBuffer);
	_ASSERT(vMipmapLevel < m_ImageCreateInfo.mipLevels);

	if (vNewImageLayout == m_CurrentImageLayoutSet[vMipmapLevel])
		return;

	vk::ImageLayout OldImageLayout = m_CurrentImageLayoutSet[vMipmapLevel];
	m_CurrentImageLayoutSet[vMipmapLevel] = vNewImageLayout;

	vk::ImageMemoryBarrier ImageMemoryBarrier = {};
	ImageMemoryBarrier.image = m_pImage;
	ImageMemoryBarrier.oldLayout = OldImageLayout;
	ImageMemoryBarrier.newLayout = vNewImageLayout;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.subresourceRange = { vImageAspectFlags,vMipmapLevel,1,0,1 };

	vk::AccessFlags srcAccessMask{};
	vk::AccessFlags dstAccessMask{};
	vk::PipelineStageFlags srcPipelineStageMask{};
	vk::PipelineStageFlags dstPipelineStageMask{};

	//TODO: srcAccessMask,dstAccessMask,srcPipelineStageMask,dstPipelineStageMask的值由OldImageLayout和vNewImageLayout的值决定
	switch (OldImageLayout)
	{
	case vk::ImageLayout::eUndefined:                     srcPipelineStageMask = vk::PipelineStageFlagBits::eHost; break;
	case vk::ImageLayout::eGeneral:                       srcAccessMask = vk::AccessFlagBits::eTransferWrite; break;
	case vk::ImageLayout::eColorAttachmentOptimal:        srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite; break;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal: srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite; break;
	case vk::ImageLayout::eDepthStencilReadOnlyOptimal:   srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead; break;
	case vk::ImageLayout::eShaderReadOnlyOptimal:         srcAccessMask = vk::AccessFlagBits::eShaderRead; break;
	case vk::ImageLayout::eTransferSrcOptimal:            srcAccessMask = vk::AccessFlagBits::eTransferRead; srcPipelineStageMask = vk::PipelineStageFlagBits::eTransfer; break;
	case vk::ImageLayout::eTransferDstOptimal:            srcAccessMask = vk::AccessFlagBits::eTransferWrite; srcPipelineStageMask = vk::PipelineStageFlagBits::eTransfer; break;
	case vk::ImageLayout::ePreinitialized:                srcAccessMask = vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eHostWrite; break;
	case vk::ImageLayout::ePresentSrcKHR:                 srcAccessMask = vk::AccessFlagBits::eMemoryRead; break;
	}

	switch (vNewImageLayout)
	{
	case vk::ImageLayout::eUndefined:                     break;
	case vk::ImageLayout::eGeneral:                       dstAccessMask = vk::AccessFlagBits::eTransferWrite; break;
	case vk::ImageLayout::eColorAttachmentOptimal:        dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite; dstPipelineStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput; break;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal: dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite; dstPipelineStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests; break;
	case vk::ImageLayout::eDepthStencilReadOnlyOptimal:   dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead; break;
	case vk::ImageLayout::eShaderReadOnlyOptimal:         dstAccessMask = vk::AccessFlagBits::eShaderRead; dstPipelineStageMask = vk::PipelineStageFlagBits::eFragmentShader; break;
	case vk::ImageLayout::eTransferSrcOptimal:            dstAccessMask = vk::AccessFlagBits::eTransferRead; dstPipelineStageMask = vk::PipelineStageFlagBits::eTransfer; break;
	case vk::ImageLayout::eTransferDstOptimal:            dstAccessMask = vk::AccessFlagBits::eTransferWrite; dstPipelineStageMask = vk::PipelineStageFlagBits::eTransfer; break;
	case vk::ImageLayout::ePreinitialized:                dstAccessMask = vk::AccessFlagBits::eTransferWrite; break;
	case vk::ImageLayout::ePresentSrcKHR:                 dstAccessMask = vk::AccessFlagBits::eMemoryRead; break;
	}

	ImageMemoryBarrier.srcAccessMask = srcAccessMask;
	ImageMemoryBarrier.dstAccessMask = dstAccessMask;

	vCommandBuffer.pipelineBarrier(srcPipelineStageMask, dstPipelineStageMask, vk::DependencyFlags{}, nullptr, nullptr, ImageMemoryBarrier);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGenericImage::translateImageLayout(vk::CommandBuffer vCommandBuffer, vk::ImageLayout vNewImageLayout, vk::ImageSubresourceRange vTranslateRange)
{
	for (auto i = vTranslateRange.baseMipLevel; i < vTranslateRange.levelCount; ++i)
	{
		translateImageLayoutAtParticularMipmapLevel(vCommandBuffer, vNewImageLayout, vTranslateRange.aspectMask, i);
	}
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGenericImage::copyFromBuffer(vk::CommandBuffer vCommandBuffer, vk::Buffer vSrcBuffer, uint32_t vMipLevel, vk::Extent3D vImageExtent, vk::DeviceSize vBufferOffset)
{
	_ASSERT(vCommandBuffer && vSrcBuffer);

	vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eColor,0,m_ImageCreateInfo.mipLevels,0,m_ImageCreateInfo.arrayLayers };
	translateImageLayout(vCommandBuffer, vk::ImageLayout::eTransferDstOptimal, TranslateRange);

	vk::BufferImageCopy CopyRegion = {};
	CopyRegion.bufferOffset = vBufferOffset;
	CopyRegion.bufferRowLength = 0;
	CopyRegion.bufferImageHeight = 0;
	CopyRegion.imageSubresource = { vk::ImageAspectFlagBits::eColor, vMipLevel, 0, 1 };
	CopyRegion.imageExtent = vImageExtent;
	CopyRegion.imageOffset = { 0,0,0 };

	vCommandBuffer.copyBufferToImage(vSrcBuffer, m_pImage, vk::ImageLayout::eTransferDstOptimal, CopyRegion);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkGenericImage::destroy()
{
	auto pDevice = CVkContext::getInstance()->getVulkanDevice();
	pDevice.destroyImage(m_pImage);
	pDevice.destroyImageView(m_pImageView);
	pDevice.freeMemory(m_pDeviceMemory);
}