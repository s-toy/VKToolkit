#pragma once
#include "Common.h"
#include <GLFW/glfw3.h>
#include "VkApplicationBase.hpp"

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
		void __createImage(uint32_t vImageWidth, uint32_t vImageHeight, uint32_t vMipmapLevel, VkSampleCountFlagBits vSampleCount, VkFormat vImageFormat, VkImageTiling vImageTiling, VkImageUsageFlags vImageUsages, VkMemoryPropertyFlags vMemoryProperties, VkImage& vImage, VkDeviceMemory& vImageDeviceMemory);
		void __transitionImageLayout(VkImage vImage, VkFormat vImageFormat, VkImageLayout vOldImageLayout, VkImageLayout vNewImageLayout, uint32_t vMipmapLevel);
		void __createFramebuffers();
		void __createTextureSamplerResources();
		void __generateMipmaps(VkImage vImage, VkFormat vImageFormat, int32_t vImageWidth, int32_t vImageHeight, uint32_t vMipmapLevel);
		void __createBuffer(VkDeviceSize vBufferSize, VkBufferUsageFlags vBufferUsage, VkMemoryPropertyFlags vMemoryProperty, VkBuffer& voBuffer, VkDeviceMemory& voBufferDeviceMemory);
		void __copyBuffer2Image(VkBuffer vBuffer, VkImage vImage, uint32_t vImageWidth, uint32_t vImageHeight);
		void __createVertexBuffer();
		void __createIndexBuffer();
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

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, VkDebugUtilsMessageTypeFlagsEXT vMessageTypeFlags, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		GLFWwindow* m_pGLFWWindow = nullptr;

		VkQueue m_pQueue = VK_NULL_HANDLE;
		VkRenderPass m_pRenderPass = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_pDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_pPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_pGraphicsPipeline = VK_NULL_HANDLE;
		VkCommandPool m_pCommandPool = VK_NULL_HANDLE;
		VkImage m_pMsaaImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pMsaaImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pMsaaImageView = VK_NULL_HANDLE;
		VkImage m_pDepthImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pDepthImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pDepthImageView = VK_NULL_HANDLE;
		VkImage m_pTextureImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pTextureImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pTextureImageView = VK_NULL_HANDLE;
		VkSampler m_pTextureSampler = VK_NULL_HANDLE;
		VkBuffer m_pVertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_pVertexBufferDeviceMemory = VK_NULL_HANDLE;
		VkBuffer m_pIndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_pIndexBufferMemory = VK_NULL_HANDLE;
		VkDescriptorPool m_pDescriptorPool = VK_NULL_HANDLE;

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

		std::vector<SVertex> m_VertexData;
		std::vector<uint32_t> m_IndexData;

		size_t m_CurrentFrame = 0;

		uint32_t m_MipmapLevel = 0;
	};
}