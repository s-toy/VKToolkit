#pragma once
#include "Common.h"
#include <GLFW/glfw3.h>
#include "VkApplicationBase.hpp"

namespace DeferredShading
{
	class CDeferredShadingApp :public hiveVKT::CVkApplicationBase
	{
	private:
		virtual bool _initV() override;
		virtual bool _renderV() override;
		virtual void _destroyV() override;

		void __cleanup();

		void __retrieveDeviceQueue();
		void __createSwapChain();
		void __retrieveSwapChainImagesAndCreateImageViews();
		void __createCommandPool();

		void __createOffScreenRenderPass();
		void __createOffScreenDescriptorSetLayout();
		void __createOffScreenPipelineLayout();
		void __createDeferredRenderPass();
		void __createDeferredDescriptorSetLayout();
		void __createDeferredPipelineLayout();
		void __createGraphicsPipelines();

		void __createOffScreenRenderTargets();
		void __createOffScreenFramebuffer();
		void __createDeferredFramebuffers();

		void __loadModel();
		//Model instance data, Quad's vertex data and Quad's index data are defined in Common.h

		void __createInstanceDataBuffer();
		void __createVertexBuffers();
		void __createIndexBuffers();

		void __createTextureSamplerResources4OffScreenRendering();
		void __createTextureSampler4DeferredRendering();

		void __createUniformBuffers();

		void __createDescriptorPool();
		void __createOffScreenDescriptorSet();
		void __createDeferredDescriptorSet();

		void __createOffScreenCommandBuffers();
		void __createDeferredCommandBuffers();

		void __createSyncObjects();

		void __updateUniformBuffer(uint32_t vImageIndex);

		void __copyBuffer(VkBuffer vSrcBuffer, VkBuffer vDstBuffer, VkDeviceSize vBufferSize);
		void __generateMipmaps(VkImage vImage, VkFormat vImageFormat, int32_t vImageWidth, int32_t vImageHeight, uint32_t vMipmapLevel);
		void __createBuffer(VkDeviceSize vBufferSize, VkBufferUsageFlags vBufferUsage, VkMemoryPropertyFlags vMemoryProperty, VkBuffer& voBuffer, VkDeviceMemory& voBufferDeviceMemory);
		void __copyBuffer2Image(VkBuffer vBuffer, VkImage vImage, uint32_t vImageWidth, uint32_t vImageHeight);
		void __createImage(uint32_t vImageWidth, uint32_t vImageHeight, uint32_t vMipmapLevel, VkSampleCountFlagBits vSampleCount, VkFormat vImageFormat, VkImageTiling vImageTiling, VkImageUsageFlags vImageUsages, VkMemoryPropertyFlags vMemoryProperties, VkImage& vImage, VkDeviceMemory& vImageDeviceMemory);
		void __transitionImageLayout(VkImage vImage, VkFormat vImageFormat, VkImageLayout vOldImageLayout, VkImageLayout vNewImageLayout, uint32_t vMipmapLevel);

		VkSurfaceFormatKHR __determineSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& vCandidateSurfaceFormatSet)const;
		VkPresentModeKHR __determinePresentMode(const std::vector<vk::PresentModeKHR>& vCandidatePresentModeSet)const;
		VkExtent2D __determineSwapChainExtent(const VkSurfaceCapabilitiesKHR& vSurfaceCapabilities)const;
		VkImageView __createImageView(const VkImage& vImage, VkFormat vImageFormat, VkImageAspectFlags vImageAspectFlags, uint32_t vMipmapLevel);
		VkFormat __findSupportedFormat(const std::vector<VkFormat>& vCandidateFormatSet, VkImageTiling vImageTiling, VkFormatFeatureFlags vFormatFeatures);
		VkShaderModule __createShaderModule(const std::vector<char>& vShaderCode);
		uint32_t __findMemoryType(uint32_t vMemoryTypeFilter, VkMemoryPropertyFlags vMemoryProperty);

		VkCommandBuffer __beginSingleTimeCommands();
		void __endSingleTimeCommands(VkCommandBuffer vCommandBuffer);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, VkDebugUtilsMessageTypeFlagsEXT vMessageTypeFlags, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		VkQueue m_pQueue = VK_NULL_HANDLE;
		VkSwapchainKHR m_pSwapChain = VK_NULL_HANDLE;
		VkCommandPool m_pCommandPool = VK_NULL_HANDLE;

		//off-screen: pipeline
		VkRenderPass m_pOffScreenRenderPass = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_pOffScreenDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_pOffScreenPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_pOffScreenPipeline = VK_NULL_HANDLE;

		//deferred: pipeline
		VkRenderPass m_pDeferredRenderPass = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_pDeferredDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_pDeferredPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_pDeferredPipeline = VK_NULL_HANDLE;

		//off-screen: render targets and frame buffer
		VkImage m_pOffScreenPositionImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pOffScreenPositionImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pOffScreenPositionImageView = VK_NULL_HANDLE;
		VkImage m_pOffScreenNormalImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pOffScreenNormalImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pOffScreenNormalImageView = VK_NULL_HANDLE;
		VkImage m_pOffScreenColorImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pOffScreenColorImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pOffScreenColorImageView = VK_NULL_HANDLE;
		VkImage m_pOffScreenDepthImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pOffScreenDepthImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pOffScreenDepthImageView = VK_NULL_HANDLE;
		VkFramebuffer m_pOffScreenFramebuffer = VK_NULL_HANDLE;

		//deferred: frame buffers
		std::vector<VkImage> m_SwapChainImageSet;
		std::vector<VkImageView> m_SwapChainImageViewSet;
		std::vector<VkFramebuffer> m_DeferredFramebufferSet;

		//model vertex data
		std::vector<SVertex> m_ModelVertexData;
		std::vector<uint32_t> m_ModelIndexData;

		//instance data buffer
		VkBuffer m_pInstanceDataBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_pInstanceDataBufferDeviceMemory = VK_NULL_HANDLE;

		//vertex buffers and index buffers
		VkBuffer m_pVertexBuffer_Model = VK_NULL_HANDLE;
		VkDeviceMemory m_pVertexBufferDeviceMemory_Model = VK_NULL_HANDLE;
		VkBuffer m_pIndexBuffer_Model = VK_NULL_HANDLE;
		VkDeviceMemory m_pIndexBufferMemory_Model = VK_NULL_HANDLE;

		VkBuffer m_pVertexBuffer_Quad = VK_NULL_HANDLE;
		VkDeviceMemory m_pVertexBufferDeviceMemory_Quad = VK_NULL_HANDLE;
		VkBuffer m_pIndexBuffer_Quad = VK_NULL_HANDLE;
		VkDeviceMemory m_pIndexBufferMemory_Quad = VK_NULL_HANDLE;

		//sampler resources for off-screen rendering
		VkImage m_pDiffuseImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pDiffuseImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pDiffuseImageView = VK_NULL_HANDLE;
		VkSampler m_pDiffuseImageSampler = VK_NULL_HANDLE;
		VkImage m_pSpecularImage = VK_NULL_HANDLE;
		VkDeviceMemory m_pSpecularImageDeviceMemory = VK_NULL_HANDLE;
		VkImageView m_pSpecularImageView = VK_NULL_HANDLE;
		VkSampler m_pSpecularSampler = VK_NULL_HANDLE;

		//sampler for deferred rendering(sample all off-screen color attachments)
		VkSampler m_pSampler4DeferredRendering = VK_NULL_HANDLE;

		//uniform buffers: off-screen pass and deferred rendering pass
		std::vector<VkBuffer> m_UniformBufferSet_OffScreen;
		std::vector<VkDeviceMemory> m_UniformBufferDeviceMemorySet_OffScreen;
		std::vector<VkBuffer> m_UniformBufferSet_Deferred;
		std::vector<VkDeviceMemory> m_UniformBufferDeviceMemorySet_Deferred;

		//descriptor set
		VkDescriptorPool m_pDescriptorPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> m_OffScreenDescriptorSet;
		std::vector<VkDescriptorSet> m_DeferredDescriptorSet;

		//command buffers
		std::vector<VkCommandBuffer> m_OffScreenCommandBufferSet;
		std::vector<VkCommandBuffer> m_DeferredCommandBufferSet;

		//sync objects
		std::vector<VkSemaphore> m_OffScreenRenderingFinishedSemaphoreSet;
		std::vector<VkSemaphore> m_ImageAvailableSemaphoreSet;
		std::vector<VkSemaphore> m_RenderFinishedSemaphoreSet;
		std::vector<VkFence> m_InFlightFenceSet;

		VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D m_SwapChainExtent = { 0,0 };

		size_t m_CurrentFrame = 0;
	};
}