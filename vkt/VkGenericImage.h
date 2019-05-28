#pragma once
#include <vulkan/vulkan.hpp>
#include "VkUtility.h"
#include "Common.h"

namespace hiveVKT
{
	class CVkGenericImage
	{
	public:
		CVkGenericImage() = default;

		void create(const vk::ImageCreateInfo& vImageCreateInfo, vk::ImageViewType vImageViewType, vk::ImageAspectFlags vImageAspectMasks, bool vHostVisible);
		void translateImageLayoutAtParticularMipmapLevel(vk::CommandBuffer vCommandBuffer, vk::ImageLayout vNewImageLayout, vk::ImageAspectFlags vImageAspectFlags, uint32_t vMipmapLevel);
		void translateImageLayout(vk::CommandBuffer vCommandBuffer, vk::ImageLayout vNewImageLayout, vk::ImageSubresourceRange vTranslateRange);
		void copyFromBuffer(vk::CommandBuffer vCommandBuffer, vk::Buffer vSrcBuffer, uint32_t vMipLevel, vk::Extent3D vImageExtent, vk::DeviceSize vBufferOffset);
		void destroy(vk::Device vDevice);

		const vk::Image&     getImage()const { return m_pImage; }
		const vk::ImageView& getImageView()const { return m_pImageView; }

	protected:
		_DISALLOW_COPY_AND_ASSIGN(CVkGenericImage);

		vk::Image				m_pImage;
		vk::ImageView			m_pImageView;
		vk::DeviceMemory		m_pDeviceMemory;
		vk::ImageCreateInfo		m_ImageCreateInfo;

		bool m_IsImageCreated = false;
		std::vector<vk::ImageLayout> m_CurrentImageLayoutSet;

	};
}