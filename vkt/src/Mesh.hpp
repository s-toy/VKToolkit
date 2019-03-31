#pragma once
#include <vulkan/vulkan.hpp>
#include "VkUtility.hpp"

namespace hiveVKT
{
	class CMesh
	{
	public:
		CMesh(vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue, const std::vector<float>& vVertexData, const std::vector<uint32_t>& vIndexData, vk::DescriptorSet vTextureDescriptorSet)
		{
			_ASSERT(vDevice && vCommandPool && vQueue);

			m_VertexData = vVertexData;
			m_IndexData = vIndexData;
			m_pTextureDescriptorSet = vTextureDescriptorSet;

			__generateBufferResource(vDevice, vCommandPool, vQueue, vVertexData, vk::BufferUsageFlagBits::eVertexBuffer, m_pVertexBuffer, m_pVertexBufferDeviceMemory);
			__generateBufferResource(vDevice, vCommandPool, vQueue, vIndexData, vk::BufferUsageFlagBits::eIndexBuffer, m_pIndexBuffer, m_pIndexBufferDeviceMemory);
		}

		void draw(const vk::CommandBuffer vCommandBuffer, const vk::PipelineLayout vPipelineLayout)
		{
			_ASSERT(vCommandBuffer && vPipelineLayout);

			vk::Buffer VertexBuffers[] = { m_pVertexBuffer };
			vk::DeviceSize Offsets[] = { 0 };

			vCommandBuffer.bindVertexBuffers(0, 1, VertexBuffers, Offsets);
			vCommandBuffer.bindIndexBuffer(m_pIndexBuffer, 0, vk::IndexType::eUint32);
			vCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vPipelineLayout, 0, m_pTextureDescriptorSet, nullptr);
			vCommandBuffer.drawIndexed(static_cast<uint32_t>(m_IndexData.size()), 1, 0, 0, 0);
		}

		void destroy(vk::Device vDevice)
		{
			vDevice.destroyBuffer(m_pVertexBuffer);
			vDevice.freeMemory(m_pVertexBufferDeviceMemory);
			vDevice.destroyBuffer(m_pIndexBuffer);
			vDevice.freeMemory(m_pIndexBufferDeviceMemory);
		}

	private:
		_DISALLOW_COPY_AND_ASSIGN(CMesh);

		std::vector<float> m_VertexData;
		std::vector<uint32_t> m_IndexData;

		vk::DescriptorSet m_pTextureDescriptorSet;

		vk::Buffer m_pVertexBuffer;
		vk::DeviceMemory m_pVertexBufferDeviceMemory;
		vk::Buffer m_pIndexBuffer;
		vk::DeviceMemory m_pIndexBufferDeviceMemory;

		template<typename T>
		void __generateBufferResource(vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue, const std::vector<T>& vBufferData, vk::BufferUsageFlags vUsage, vk::Buffer& voBuffer, vk::DeviceMemory& voBufferDeviceMemory)
		{
			vk::DeviceSize BufferSize = sizeof(T) * vBufferData.size();

			vk::Buffer pStagingBuffer;
			vk::DeviceMemory pStagingBufferDeviceMemory;

			createBuffer(vDevice, BufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pStagingBuffer, pStagingBufferDeviceMemory);

			void* Data = nullptr;
			vkMapMemory(vDevice, pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
			memcpy(Data, vBufferData.data(), static_cast<size_t>(BufferSize));
			vkUnmapMemory(vDevice, pStagingBufferDeviceMemory);

			createBuffer(vDevice, BufferSize, vk::BufferUsageFlagBits::eTransferDst | vUsage, vk::MemoryPropertyFlagBits::eDeviceLocal, voBuffer, voBufferDeviceMemory);

			executeImmediately(vDevice, vCommandPool, vQueue, [&](vk::CommandBuffer vCommandBuffer) {
				vk::BufferCopy CopyRegion = {};
				CopyRegion.size = BufferSize;
				CopyRegion.srcOffset = 0;
				CopyRegion.dstOffset = 0;
				vCommandBuffer.copyBuffer(pStagingBuffer, voBuffer, 1, &CopyRegion);
			});

			vkDestroyBuffer(vDevice, pStagingBuffer, nullptr);
			vkFreeMemory(vDevice, pStagingBufferDeviceMemory, nullptr);
		}
	};
}