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
		virtual void _awakeV()override;
		virtual bool _initV() override;
		virtual void _updateV() override;
		virtual void _destroyV() override;

		void __createRenderPass();
		void __createDescriptorSetLayout();
		void __createPipelineLayout();
		void __createGraphicsPipeline();

		void __createMsaaResource();
		void __createDepthResources();
		void __createFramebuffers();
		void __generateMipmaps(hiveVKT::CVkGenericImage& vTexture, int32_t vTextureWidth, int32_t vTextureHeight, uint32_t vMipmapLevel, vk::Format vTextureFormat);
		void __createUniformBuffers();

		void __createDescriptorPool();
		void __createDescriptorSet();

		void __createCommandBuffers();
		void __createSyncObjects();
		void __updateUniformBuffer(uint32_t vImageIndex);
		void __loadModel();

		VkFormat __findSupportedFormat(const std::vector<VkFormat>& vCandidateFormatSet, VkImageTiling vImageTiling, VkFormatFeatureFlags vFormatFeatures);
		VkSampleCountFlagBits __getMaxSampleCount();

		GLFWwindow* m_pGLFWWindow = nullptr;

		VkRenderPass m_pRenderPass = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_pDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_pPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_pGraphicsPipeline = VK_NULL_HANDLE;
		VkDescriptorPool m_pDescriptorPool = VK_NULL_HANDLE;
		hiveVKT::CVkGenericImage m_MsaaAttachment;
		hiveVKT::CVkGenericImage m_DepthAttachment;
		hiveVKT::CModel* m_pModel = nullptr;

		VkSampleCountFlagBits m_SampleCount = VK_SAMPLE_COUNT_1_BIT;

		std::vector<VkFramebuffer> m_FramebufferSet;
		std::vector<vk::Buffer> m_UniformBufferSet;
		std::vector<vk::DeviceMemory> m_UniformBufferDeviceMemorySet;
		std::vector<VkDescriptorSet> m_DescriptorSet;
		std::vector<VkCommandBuffer> m_CommandBufferSet;
		std::vector<VkSemaphore> m_ImageAvailableSemaphoreSet;
		std::vector<VkSemaphore> m_RenderFinishedSemaphoreSet;
		std::vector<VkFence> m_InFlightFenceSet;

		size_t m_CurrentFrame = 0;

		uint32_t m_MipmapLevel = 0;
	};
}