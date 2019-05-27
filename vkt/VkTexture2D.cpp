#include "VkTexture2D.h"

using namespace hiveVKT;

hiveVKT::CVkTexture2D::CVkTexture2D()
{
	m_ImageCreateInfo.imageType = vk::ImageType::e2D;
	m_ImageCreateInfo.mipLevels = 1;
	m_ImageCreateInfo.arrayLayers = 1;
	m_ImageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	m_ImageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	m_ImageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	m_ImageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	m_ImageCreateInfo.samples = vk::SampleCountFlagBits::e1;

	m_SamplerCreateInfo.magFilter = vk::Filter::eLinear;
	m_SamplerCreateInfo.minFilter = vk::Filter::eLinear;
	m_SamplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	m_SamplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	m_SamplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	m_SamplerCreateInfo.anisotropyEnable = false;
	m_SamplerCreateInfo.maxAnisotropy = 0.0f;
	m_SamplerCreateInfo.borderColor = vk::BorderColor::eIntTransparentBlack;
	m_SamplerCreateInfo.unnormalizedCoordinates = false;
	m_SamplerCreateInfo.compareEnable = false;
	m_SamplerCreateInfo.compareOp = vk::CompareOp::eAlways;
	m_SamplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
	m_SamplerCreateInfo.mipLodBias = 0.0f;
	m_SamplerCreateInfo.maxLod = 1.0f;
	m_SamplerCreateInfo.minLod = 0.0f;
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkTexture2D::create(vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue, int vTextureWidth, int vTextureHeight, vk::Format vTextureFormat, uint32_t vTextureMipLevel, vk::DeviceSize vSize, unsigned char* vPixel)
{
	_ASSERT(vDevice && vCommandPool && vQueue);

	uint32_t MaxMipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(vTextureWidth, vTextureHeight)))) + 1;

	m_ImageCreateInfo.extent = vk::Extent3D{ static_cast<uint32_t>(vTextureWidth), static_cast<uint32_t>(vTextureHeight), 1 };
	m_ImageCreateInfo.mipLevels = std::min(vTextureMipLevel, MaxMipLevel);
	m_ImageCreateInfo.format = vTextureFormat;

	m_SamplerCreateInfo.maxLod = static_cast<float>(m_ImageCreateInfo.mipLevels);

	CVkGenericImage::create(vDevice, m_ImageCreateInfo, vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor, false);

	vk::Buffer pStagingBuffer;
	vk::DeviceMemory pStagingBufferDeviceMemory;

	createBuffer(vDevice, vSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(vDevice, pStagingBufferDeviceMemory, 0, vSize, 0, &Data);
	memcpy(Data, vPixel, static_cast<size_t>(vSize));
	vkUnmapMemory(vDevice, pStagingBufferDeviceMemory);

	//将像素从stage buffer拷贝到image的第0个mipmap
	executeImmediately(vDevice, vCommandPool, vQueue, [&](vk::CommandBuffer vCommandBuffer) {
		vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eColor,0,m_ImageCreateInfo.mipLevels,0,m_ImageCreateInfo.arrayLayers };
		translateImageLayout(vCommandBuffer, vk::ImageLayout::eTransferDstOptimal, TranslateRange);

		vk::BufferImageCopy CopyRegion = {};
		CopyRegion.bufferOffset = 0;
		CopyRegion.bufferRowLength = 0;
		CopyRegion.bufferImageHeight = 0;
		CopyRegion.imageSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };
		CopyRegion.imageExtent = m_ImageCreateInfo.extent;
		CopyRegion.imageOffset = { 0,0,0 };

		vCommandBuffer.copyBufferToImage(pStagingBuffer, m_pImage, vk::ImageLayout::eTransferDstOptimal, CopyRegion);
		});

	vDevice.destroyBuffer(pStagingBuffer);
	vDevice.freeMemory(pStagingBufferDeviceMemory);

	//由第0个mipmap生成其余的mipmap的像素
	if (m_ImageCreateInfo.mipLevels > 1)
	{
		int MipWidth = m_ImageCreateInfo.extent.width;
		int MipHeight = m_ImageCreateInfo.extent.height;

		executeImmediately(vDevice, vCommandPool, vQueue, [&](vk::CommandBuffer vCommandBuffer) {
			for (uint32_t i = 1; i < m_ImageCreateInfo.mipLevels; ++i)
			{
				translateImageLayoutAtParticularMipmapLevel(vCommandBuffer, vk::ImageLayout::eTransferSrcOptimal, vk::ImageAspectFlagBits::eColor, i - 1);

				vk::ImageBlit ImageBlit = {};
				ImageBlit.srcOffsets[0] = { 0,0,0 };
				ImageBlit.srcOffsets[1] = { MipWidth,MipHeight,1 };
				ImageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				ImageBlit.srcSubresource.mipLevel = i - 1;
				ImageBlit.srcSubresource.baseArrayLayer = 0;
				ImageBlit.srcSubresource.layerCount = 1;
				ImageBlit.dstOffsets[0] = { 0,0,0 };
				ImageBlit.dstOffsets[1] = { MipWidth > 1 ? MipWidth / 2 : 1, MipHeight > 1 ? MipHeight / 2 : 1, 1 };
				ImageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				ImageBlit.dstSubresource.mipLevel = i;
				ImageBlit.dstSubresource.baseArrayLayer = 0;
				ImageBlit.dstSubresource.layerCount = 1;

				vCommandBuffer.blitImage(m_pImage, vk::ImageLayout::eTransferSrcOptimal, m_pImage, vk::ImageLayout::eTransferDstOptimal, ImageBlit, vk::Filter::eLinear);

				translateImageLayoutAtParticularMipmapLevel(vCommandBuffer, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor, i - 1);

				if (MipWidth > 1) MipWidth /= 2;
				if (MipHeight > 1) MipHeight /= 2;
			}

			translateImageLayoutAtParticularMipmapLevel(vCommandBuffer, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor, m_ImageCreateInfo.mipLevels - 1);
			});
	}

	m_pSampler = vDevice.createSampler(m_SamplerCreateInfo);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkTexture2D::destroy(vk::Device vDevice)
{
	vDevice.destroyImage(m_pImage);
	vDevice.destroyImageView(m_pImageView);
	vDevice.freeMemory(m_pDeviceMemory);
	vDevice.destroySampler(m_pSampler);
}