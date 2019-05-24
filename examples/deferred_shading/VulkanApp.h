#pragma once
#include "Common.h"
#include <GLFW/glfw3.h>
#include "VkApplicationBase.hpp"
#include "VKGenericImage.hpp"

namespace hiveVKT
{
	class CModel;
}

namespace DeferredShading
{
	class CDeferredShadingApp :public hiveVKT::CVkApplicationBase
	{
	private:
		virtual void _awakeV() override;
		virtual bool _initV() override;
		virtual void _updateV() override;
		virtual void _destroyV() override;

		void __cleanup();

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
		void __createVertexBuffers();
		void __createIndexBuffers();

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

		VkFormat __findSupportedFormat(const std::vector<VkFormat>& vCandidateFormatSet, VkImageTiling vImageTiling, VkFormatFeatureFlags vFormatFeatures);

		VkRenderPass m_pOffScreenRenderPass = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_pOffScreenDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_pOffScreenPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_pOffScreenPipeline = VK_NULL_HANDLE;

		VkRenderPass m_pDeferredRenderPass = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_pDeferredDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_pDeferredPipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_pDeferredPipeline = VK_NULL_HANDLE;

		VkFramebuffer m_pOffScreenFramebuffer = VK_NULL_HANDLE;
		hiveVKT::CVkGenericImage m_PositionAttachment;
		hiveVKT::CVkGenericImage m_NormalAttachment;
		hiveVKT::CVkGenericImage m_ColorAttachment;
		hiveVKT::CVkGenericImage m_DepthAttachment;

		//deferred: frame buffers
		std::vector<VkFramebuffer> m_DeferredFramebufferSet;

		hiveVKT::CModel* m_pModel = nullptr;

		vk::Buffer m_pVertexBuffer_Quad = nullptr;
		vk::DeviceMemory m_pVertexBufferDeviceMemory_Quad = nullptr;
		vk::Buffer m_pIndexBuffer_Quad = nullptr;
		vk::DeviceMemory m_pIndexBufferMemory_Quad = nullptr;

		//sampler for deferred rendering(sample all off-screen color attachments)
		VkSampler m_pSampler4DeferredRendering = VK_NULL_HANDLE;

		//uniform buffers: off-screen pass and deferred rendering pass
		std::vector<vk::Buffer> m_UniformBufferSet_OffScreen;
		std::vector<vk::DeviceMemory> m_UniformBufferDeviceMemorySet_OffScreen;
		std::vector<vk::Buffer> m_UniformBufferSet_Deferred;
		std::vector<vk::DeviceMemory> m_UniformBufferDeviceMemorySet_Deferred;

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

		size_t m_CurrentFrame = 0;
	};
}