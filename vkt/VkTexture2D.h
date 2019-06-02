#pragma once
#include "VkGenericImage.h"
#include "Export.h"

namespace hiveVKT
{
	class VKT_DECLSPEC CVkTexture2D :public CVkGenericImage
	{
	public:
		CVkTexture2D();

		void setSamplerFilterMode(vk::Filter vMagFilter, vk::Filter vMinFilter)
		{
			m_SamplerCreateInfo.magFilter = vMagFilter;
			m_SamplerCreateInfo.minFilter = vMinFilter;
		}
		void setSamplerAddressMode(vk::SamplerAddressMode vAddressMode)
		{
			m_SamplerCreateInfo.addressModeU = vAddressMode;
			m_SamplerCreateInfo.addressModeV = vAddressMode;
			m_SamplerCreateInfo.addressModeW = vAddressMode;
		}
		void setSamplerAnisotropyHint(bool vEnable, float vMaxAnisotropy)
		{
			m_SamplerCreateInfo.anisotropyEnable = vEnable;
			m_SamplerCreateInfo.maxAnisotropy = vMaxAnisotropy;
		}
		void setSamplerBorderColor(vk::BorderColor vBorderColor)
		{
			m_SamplerCreateInfo.borderColor = vBorderColor;
		}
		void setSamplerUnnormalizedCoordinatesHint(bool vHint)
		{
			m_SamplerCreateInfo.unnormalizedCoordinates = vHint;
		}
		void setSamplerCompareHint(bool vEnable, vk::CompareOp vCompareOp)
		{
			m_SamplerCreateInfo.compareEnable = vEnable;
			m_SamplerCreateInfo.compareOp = vCompareOp;
		}
		void setSamplerMipmapMode(vk::SamplerMipmapMode vSamplerMipmapMode)
		{
			m_SamplerCreateInfo.mipmapMode = vSamplerMipmapMode;
		}
		void setSamplerLoD(float vMaxLoD, float vMinLoD)
		{
			m_SamplerCreateInfo.maxLod = vMaxLoD;
			m_SamplerCreateInfo.minLod = vMinLoD;
		}
		void setSamplerLoDBias(float vMipLoDBias)
		{
			m_SamplerCreateInfo.mipLodBias = vMipLoDBias;
		}

		void create(int vTextureWidth, int vTextureHeight, vk::Format vTextureFormat, uint32_t vTextureMipLevel, vk::DeviceSize vSize, unsigned char* vPixel);

		const vk::Sampler& getSampler()const { return m_pSampler; }

		void destroy(vk::Device vDevice);

	private:
		_DISALLOW_COPY_AND_ASSIGN(CVkTexture2D);

		vk::Sampler m_pSampler;
		vk::SamplerCreateInfo m_SamplerCreateInfo;
	};
}