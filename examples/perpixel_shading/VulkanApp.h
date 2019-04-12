#pragma once
#include "Common.h"
#include <GLFW/glfw3.h>
#include "VkApplicationBase.hpp"
#include "VkGenericImage.hpp"

namespace hiveVKT
{
	class CModel;
}

namespace VulkanApp
{
	class CPerpixelShadingApp : public hiveVKT::CVkApplicationBase
	{
	private:
		virtual bool _initV() override;
		virtual bool _renderV() override;
		virtual void _destroyV() override;

		void __retrieveDeviceQueue();
		void __retrieveSwapChainImagesAndCreateImageViews();
		void __createRenderPass();
		void __createDescriptorSetLayout();
		void __createPipelineLayout();
		void __createGraphicsPipeline();
		void __createCommandPool();
		void __createMsaaResource();
		void __createDepthResources();
		void __createFramebuffers();
		void __generateMipmaps(hiveVKT::CVkGenericImage& vTexture, int32_t vTextureWidth, int32_t vTextureHeight, uint32_t vMipmapLevel, vk::Format vTextureFormat);
		void __createBuffer(VkDeviceSize vBufferSize, VkBufferUsageFlags vBufferUsage, VkMemoryPropertyFlags vMemoryProperty, VkBuffer& voBuffer, VkDeviceMemory& voBufferDeviceMemory);
		void __createUniformBuffers();
		void __createDescriptorPool();
		void __createDescriptorSet();
		void __createCommandBuffers();
		void __createSyncObjects();
		void __copyBuffer(VkBuffer vSrcBuffer, VkBuffer vDstBuffer, VkDeviceSize vBufferSize);
		void __updateUniformBuffer(uint32_t vImageIndex);
		void __loadModel();

		VkImageView __createImageView(const VkImage& vImage, VkFormat vImageFormat, VkImageAspectFlags vImageAspectFlags, uint32_t vMipmapLevel);
		VkFormat __findSupportedFormat(const std::vector<VkFormat>& vCandidateFormatSet, VkImageTiling vImageTiling, VkFormatFeatureFlags vFormatFeatures);
		uint32_t __findMemoryType(uint32_t vMemoryTypeFilter, VkMemoryPropertyFlags vMemoryProperty);
		VkSampleCountFlagBits __getMaxSampleCount();

		VkCommandBuffer __beginSingleTimeCommands();
		void __endSingleTimeCommands(VkCommandBuffer vCommandBuffer);

		GLFWwindow* m_pGLFWWindow = nullptr;

		VkQueue m_pQueue = VK_NULL_HANDLE;
		VkRenderPass m_pRenderPass = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_pDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_pPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_pGraphicsPipeline = VK_NULL_HANDLE;
		VkCommandPool m_pCommandPool = VK_NULL_HANDLE;
		VkDescriptorPool m_pDescriptorPool = VK_NULL_HANDLE;
		hiveVKT::CVkGenericImage m_MsaaAttachment;
		hiveVKT::CVkGenericImage m_DepthAttachment;
		hiveVKT::CModel* m_pModel = nullptr;

		VkSampleCountFlagBits m_SampleCount = VK_SAMPLE_COUNT_1_BIT;

		std::vector<VkImage> m_SwapChainImageSet;
		std::vector<VkImageView> m_SwapChainImageViewSet;
		std::vector<VkFramebuffer> m_FramebufferSet;
		std::vector<VkBuffer> m_UniformBufferSet;
		std::vector<VkDeviceMemory> m_UniformBufferDeviceMemorySet;
		std::vector<VkDescriptorSet> m_DescriptorSet;
		std::vector<VkCommandBuffer> m_CommandBufferSet;
		std::vector<VkSemaphore> m_ImageAvailableSemaphoreSet;
		std::vector<VkSemaphore> m_RenderFinishedSemaphoreSet;
		std::vector<VkFence> m_InFlightFenceSet;

		size_t m_CurrentFrame = 0;

		uint32_t m_MipmapLevel = 0;
	};
}