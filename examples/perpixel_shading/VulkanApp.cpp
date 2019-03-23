#include "VulkanApp.h"
#include <set>
#include <iostream>
#include <chrono>
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "../external/tiny_obj_loader.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/gtc/matrix_transform.hpp>
#include "VkGraphicsPipelineCreator.hpp"
#include "VkShaderModuleCreator.hpp"
#include "VkDeviceCreator.hpp"
#include "VkRenderPassCreator.hpp"

using namespace hiveVKT;

//************************************************************************************
//Function:
bool VulkanApp::CPerpixelShadingApp::_initV()
{
	if (!CVkApplicationBase::_initV()) return false;

	m_SampleCount = __getMaxSampleCount();

	__retrieveDeviceQueue();
	__retrieveSwapChainImagesAndCreateImageViews();
	__createRenderPass();
	__createDescriptorSetLayout();
	__createPipelineLayout();
	__createGraphicsPipeline();
	__createCommandPool();
	__createMsaaResource();
	__createDepthResources();
	__createFramebuffers();
	__loadModel();
	__createTextureSamplerResources();
	__createVertexBuffer();
	__createIndexBuffer();
	__createUniformBuffers();
	__createDescriptorPool();
	__createDescriptorSet();
	__createCommandBuffers();
	__createSyncObjects();

	return true;
}

//************************************************************************************
//Function:
bool VulkanApp::CPerpixelShadingApp::_renderV()
{
	if (!CVkApplicationBase::_renderV()) return false;

	vkWaitForFences(_device(), 1, &m_InFlightFenceSet[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t ImageIndex = 0;
	VkResult Result = vkAcquireNextImageKHR(_device(), _swapchain(), std::numeric_limits<uint64_t>::max(), m_ImageAvailableSemaphoreSet[m_CurrentFrame], VK_NULL_HANDLE, &ImageIndex);
	if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		throw std::runtime_error("Failed to acquire swap chain image!");
	else if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("Failed to acquire swap chain image!");

	__updateUniformBuffer(ImageIndex);

	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore WaitSemaphores[] = { m_ImageAvailableSemaphoreSet[m_CurrentFrame] };
	VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = WaitSemaphores;
	SubmitInfo.pWaitDstStageMask = WaitStages;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &m_CommandBufferSet[ImageIndex];
	VkSemaphore SignalSemaphores[] = { m_RenderFinishedSemaphoreSet[m_CurrentFrame] };
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = SignalSemaphores;
	vkResetFences(_device(), 1, &m_InFlightFenceSet[m_CurrentFrame]);
	if (vkQueueSubmit(m_pQueue, 1, &SubmitInfo, m_InFlightFenceSet[m_CurrentFrame]) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer!");

	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores;
	VkSwapchainKHR SwapChains[] = { _swapchain() };
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = SwapChains;
	PresentInfo.pImageIndices = &ImageIndex;
	PresentInfo.pResults = nullptr;
	Result = vkQueuePresentKHR(m_pQueue, &PresentInfo);
	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("Failed to present swap chain image!");
	else if (Result != VK_SUCCESS)
		throw std::runtime_error("Failed to present swap chain image!");

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return true;
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::_destroyV()
{
	_device().waitIdle();

	for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(_device(), m_ImageAvailableSemaphoreSet[i], nullptr);
		vkDestroySemaphore(_device(), m_RenderFinishedSemaphoreSet[i], nullptr);
		vkDestroyFence(_device(), m_InFlightFenceSet[i], nullptr);
	}

	vkDestroyDescriptorPool(_device(), m_pDescriptorPool, nullptr);

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		vkDestroyBuffer(_device(), m_UniformBufferSet[i], nullptr);
		vkFreeMemory(_device(), m_UniformBufferDeviceMemorySet[i], nullptr);
	}

	vkDestroyBuffer(_device(), m_pIndexBuffer, nullptr);
	vkFreeMemory(_device(), m_pIndexBufferMemory, nullptr);
	vkDestroyBuffer(_device(), m_pVertexBuffer, nullptr);
	vkFreeMemory(_device(), m_pVertexBufferDeviceMemory, nullptr);

	for (auto i = 0; i < m_FramebufferSet.size(); ++i)
		vkDestroyFramebuffer(_device(), m_FramebufferSet[i], nullptr);

	vkDestroyCommandPool(_device(), m_pCommandPool, nullptr);

	vkDestroyPipeline(_device(), m_pGraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(_device(), m_pPipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(_device(), m_pDescriptorSetLayout, nullptr);
	vkDestroyRenderPass(_device(), m_pRenderPass, nullptr);

	for (auto i = 0; i < m_SwapChainImageViewSet.size(); ++i)
		vkDestroyImageView(_device(), m_SwapChainImageViewSet[i], nullptr);

	m_MsaaAttachment.destroy(_device());
	m_DepthAttachment.destroy(_device());
	m_Texture.destroy(_device());
	vkDestroySampler(_device(), m_pTextureSampler, nullptr);

	CVkApplicationBase::_destroyV();
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__retrieveDeviceQueue()
{
	SQueueFamilyIndices QueueFamilyIndices = _requiredQueueFamilyIndices();
	vkGetDeviceQueue(_device(), QueueFamilyIndices.QueueFamily.value(), 0, &m_pQueue);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__retrieveSwapChainImagesAndCreateImageViews()
{
	uint32_t SwapChainImageCount = 0;
	vkGetSwapchainImagesKHR(_device(), _swapchain(), &SwapChainImageCount, nullptr);
	m_SwapChainImageSet.resize(SwapChainImageCount);
	vkGetSwapchainImagesKHR(_device(), _swapchain(), &SwapChainImageCount, m_SwapChainImageSet.data());

	m_SwapChainImageViewSet.resize(SwapChainImageCount);

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		m_SwapChainImageViewSet[i] = __createImageView(m_SwapChainImageSet[i], (VkFormat)_swapchainImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createRenderPass()
{
	hiveVKT::CVkRenderPassCreator RenderPassCreator;

	auto DepthAttachmentFormat = __findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	RenderPassCreator.addAttachment(_swapchainImageFormat(), vk::ImageLayout::eColorAttachmentOptimal, (vk::SampleCountFlagBits)m_SampleCount);
	RenderPassCreator.addAttachment((vk::Format)DepthAttachmentFormat, vk::ImageLayout::eDepthStencilAttachmentOptimal, (vk::SampleCountFlagBits)m_SampleCount);
	RenderPassCreator.addAttachment(_swapchainImageFormat(), vk::ImageLayout::ePresentSrcKHR);
	RenderPassCreator.fetchLastAttachment().setLoadOp(vk::AttachmentLoadOp::eDontCare);

	std::vector<vk::AttachmentReference> ColorAttachmentRefs = { {0, vk::ImageLayout::eColorAttachmentOptimal} };
	vk::AttachmentReference DepthStencilAttachmentRef = { 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };
	std::vector<vk::AttachmentReference> ResolveAttachmentRefs = { {2, vk::ImageLayout::ePresentSrcKHR} };
	RenderPassCreator.addSubpass(ColorAttachmentRefs, DepthStencilAttachmentRef, ResolveAttachmentRefs, {}, {});

	m_pRenderPass = RenderPassCreator.create(_device());
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding UniformBufferBinding = {};
	UniformBufferBinding.binding = 0;
	UniformBufferBinding.descriptorCount = 1;
	UniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	UniformBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	UniformBufferBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding SamplerBinding = {};
	SamplerBinding.binding = 1;
	SamplerBinding.descriptorCount = 1;
	SamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	SamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	UniformBufferBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 2> Bindings = { UniformBufferBinding,SamplerBinding };

	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
	DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
	DescriptorSetLayoutCreateInfo.pBindings = Bindings.data();

	if (vkCreateDescriptorSetLayout(_device(), &DescriptorSetLayoutCreateInfo, nullptr, &m_pDescriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor set layout!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createPipelineLayout()
{
	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutCreateInfo.setLayoutCount = 1;
	PipelineLayoutCreateInfo.pSetLayouts = &m_pDescriptorSetLayout;
	PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(_device(), &PipelineLayoutCreateInfo, nullptr, &m_pPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createGraphicsPipeline()
{
	hiveVKT::CVkShaderModuleCreator ShaderModuleCreator;
	auto VertexShaderModule = ShaderModuleCreator.createUnique(_device(), "vert.spv");
	auto FragmentShaderModule = ShaderModuleCreator.createUnique(_device(), "frag.spv");

	hiveVKT::CVkGraphicsPipelineCreator PipelineCreator(_swapchainExtent().width, _swapchainExtent().height);

	PipelineCreator.addShaderStage(vk::ShaderStageFlagBits::eVertex, VertexShaderModule.get());
	PipelineCreator.addShaderStage(vk::ShaderStageFlagBits::eFragment, FragmentShaderModule.get());

	PipelineCreator.addVertexBinding({ 0, sizeof(SVertex) , vk::VertexInputRate::eVertex });
	PipelineCreator.addVertexAttribute({ 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(SVertex, SVertex::Position) });
	PipelineCreator.addVertexAttribute({ 1, 0, vk::Format::eR32G32B32Sfloat , offsetof(SVertex, SVertex::Color) });
	PipelineCreator.addVertexAttribute({ 2, 0, vk::Format::eR32G32Sfloat , offsetof(SVertex, SVertex::TexCoord) });

	PipelineCreator.fetchRasterizationState().setFrontFace(vk::FrontFace::eCounterClockwise);

	PipelineCreator.setMultisampleState({ {}, vk::SampleCountFlagBits(m_SampleCount), VK_TRUE, .2f, nullptr, VK_FALSE, VK_FALSE });

	PipelineCreator.fetchDepthStencilState().setDepthTestEnable(VK_TRUE);
	PipelineCreator.fetchDepthStencilState().setDepthCompareOp(vk::CompareOp::eLess);

	m_pGraphicsPipeline = PipelineCreator.create(_device(), m_pPipelineLayout, nullptr, m_pRenderPass);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createCommandPool()
{
	SQueueFamilyIndices QueueFamilyIndices = _requiredQueueFamilyIndices();

	VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndices.QueueFamily.value();

	if (vkCreateCommandPool(_device(), &CommandPoolCreateInfo, nullptr, &m_pCommandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createMsaaResource()
{
	vk::ImageCreateInfo ImageCreateInfo = {};
	ImageCreateInfo.imageType = vk::ImageType::e2D;
	ImageCreateInfo.extent = vk::Extent3D{ _swapchainExtent().width, _swapchainExtent().height, 1 };
	ImageCreateInfo.mipLevels = 1;
	ImageCreateInfo.arrayLayers = 1;
	ImageCreateInfo.format = _swapchainImageFormat();
	ImageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	ImageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	ImageCreateInfo.usage = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment;
	ImageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	ImageCreateInfo.samples = static_cast<vk::SampleCountFlagBits>(m_SampleCount);

	m_MsaaAttachment.create(_device(), ImageCreateInfo, vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor, _PhysicalDeviceMemoryProperties(), false);
	vk::CommandBuffer CommandBuffer = __beginSingleTimeCommands();
	vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eColor,0,1,0,1 };
	m_MsaaAttachment.translateImageLayout(CommandBuffer, vk::ImageLayout::eColorAttachmentOptimal, TranslateRange);
	__endSingleTimeCommands(CommandBuffer);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createDepthResources()
{
	VkFormat DepthImageFormat = __findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	vk::ImageCreateInfo ImageCreateInfo = {};
	ImageCreateInfo.imageType = vk::ImageType::e2D;
	ImageCreateInfo.extent = vk::Extent3D{ _swapchainExtent().width, _swapchainExtent().height, 1 };
	ImageCreateInfo.mipLevels = 1;
	ImageCreateInfo.arrayLayers = 1;
	ImageCreateInfo.format = static_cast<vk::Format>(DepthImageFormat);
	ImageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	ImageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	ImageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	ImageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	ImageCreateInfo.samples = static_cast<vk::SampleCountFlagBits>(m_SampleCount);

	m_DepthAttachment.create(_device(), ImageCreateInfo, vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eDepth, _PhysicalDeviceMemoryProperties(), false);
	vk::CommandBuffer CommandBuffer = __beginSingleTimeCommands();
	vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eDepth,0,1,0,1 };
	m_DepthAttachment.translateImageLayout(CommandBuffer, vk::ImageLayout::eDepthStencilAttachmentOptimal, TranslateRange);
	__endSingleTimeCommands(CommandBuffer);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createFramebuffers()
{
	m_FramebufferSet.resize(m_SwapChainImageSet.size());

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		std::array<VkImageView, 3> Attachments = { m_MsaaAttachment.getImageView(),m_DepthAttachment.getImageView(),m_SwapChainImageViewSet[i] };

		VkFramebufferCreateInfo FramebufferCreateInfo = {};
		FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferCreateInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
		FramebufferCreateInfo.pAttachments = Attachments.data();
		FramebufferCreateInfo.renderPass = m_pRenderPass;
		FramebufferCreateInfo.layers = 1;
		FramebufferCreateInfo.width = _swapchainExtent().width;
		FramebufferCreateInfo.height = _swapchainExtent().height;

		if (vkCreateFramebuffer(_device(), &FramebufferCreateInfo, nullptr, &m_FramebufferSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create frame buffer!");
	}
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createTextureSamplerResources()
{
	int TextureWidth = 0, TextureHeight = 0, TextureChannels = 0;
	unsigned char* Pixels = stbi_load("../../resource/models/cyborg/cyborg_diffuse.png", &TextureWidth, &TextureHeight, &TextureChannels, STBI_rgb_alpha);
	if (!Pixels)
		throw std::runtime_error("Failed to load texture image!");

	m_MipmapLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(TextureWidth, TextureHeight)))) + 1;

	VkDeviceSize ImageSize = TextureWidth * TextureHeight * 4;

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);
	void* Data = nullptr;
	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, ImageSize, 0, &Data);
	memcpy(Data, Pixels, static_cast<size_t>(ImageSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	stbi_image_free(Pixels);

	vk::ImageCreateInfo ImageCreateInfo = {};
	ImageCreateInfo.imageType = vk::ImageType::e2D;
	ImageCreateInfo.extent = vk::Extent3D{ static_cast<uint32_t>(TextureWidth), static_cast<uint32_t>(TextureHeight), 1 };
	ImageCreateInfo.mipLevels = m_MipmapLevel;
	ImageCreateInfo.arrayLayers = 1;
	ImageCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
	ImageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	ImageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	ImageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	ImageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	ImageCreateInfo.samples = vk::SampleCountFlagBits::e1;

	m_Texture.create(_device(), ImageCreateInfo, vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor, _PhysicalDeviceMemoryProperties(), false);
	vk::CommandBuffer CommandBuffer = __beginSingleTimeCommands();
	vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eColor,0,m_MipmapLevel,0,1 };
	m_Texture.translateImageLayout(CommandBuffer, vk::ImageLayout::eTransferDstOptimal, TranslateRange);
	m_Texture.copyFromBuffer(CommandBuffer, pStagingBuffer, 0, { static_cast<uint32_t>(TextureWidth),static_cast<uint32_t>(TextureHeight),1u }, 0);
	__endSingleTimeCommands(CommandBuffer);
	__generateMipmaps(m_Texture, TextureWidth, TextureHeight, m_MipmapLevel, vk::Format::eR8G8B8A8Unorm);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);

	VkSamplerCreateInfo SamplerCreateInfo = {};
	SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	SamplerCreateInfo.anisotropyEnable = VK_TRUE;
	SamplerCreateInfo.maxAnisotropy = 16;
	SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
	SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	SamplerCreateInfo.compareEnable = VK_FALSE;
	SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	SamplerCreateInfo.mipLodBias = 0.0f;
	SamplerCreateInfo.maxLod = static_cast<float>(m_MipmapLevel);
	SamplerCreateInfo.minLod = 0.0f;

	if (vkCreateSampler(_device(), &SamplerCreateInfo, nullptr, &m_pTextureSampler) != VK_SUCCESS)
		throw std::runtime_error("Failed to create texture sampler!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__generateMipmaps(hiveVKT::CVKGenericImage& vTexture, int32_t vTextureWidth, int32_t vTextureHeight, uint32_t vMipmapLevel, vk::Format vTextureFormat)
{
	VkFormatProperties FormatProperties;
	vkGetPhysicalDeviceFormatProperties(_physicalDevice(), static_cast<VkFormat>(vTextureFormat), &FormatProperties);
	if (!(FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		throw std::runtime_error("Texture image format does not support linear blitting!");

	int32_t MipmapWidth = vTextureWidth;
	int32_t MipmapHeight = vTextureHeight;

	VkCommandBuffer CommandBuffer = __beginSingleTimeCommands();

	for (uint32_t i = 1; i < vMipmapLevel; ++i)
	{
		vTexture.translateImageLayoutAtParticularMipmapLevel(CommandBuffer, vk::ImageLayout::eTransferSrcOptimal, vk::ImageAspectFlagBits::eColor, i - 1);

		VkImageBlit ImageBlit = {};
		ImageBlit.srcOffsets[0] = { 0,0,0 };
		ImageBlit.srcOffsets[1] = { MipmapWidth,MipmapHeight,1 };
		ImageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageBlit.srcSubresource.mipLevel = i - 1;
		ImageBlit.srcSubresource.baseArrayLayer = 0;
		ImageBlit.srcSubresource.layerCount = 1;
		ImageBlit.dstOffsets[0] = { 0,0,0 };
		ImageBlit.dstOffsets[1] = { MipmapWidth > 1 ? MipmapWidth / 2 : 1, MipmapHeight > 1 ? MipmapHeight / 2 : 1, 1 };
		ImageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageBlit.dstSubresource.mipLevel = i;
		ImageBlit.dstSubresource.baseArrayLayer = 0;
		ImageBlit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(CommandBuffer, vTexture.getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vTexture.getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &ImageBlit, VK_FILTER_LINEAR);

		vTexture.translateImageLayoutAtParticularMipmapLevel(CommandBuffer, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor, i - 1);

		if (MipmapWidth > 1) MipmapWidth /= 2;
		if (MipmapHeight > 1) MipmapHeight /= 2;
	}

	vTexture.translateImageLayoutAtParticularMipmapLevel(CommandBuffer, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor, vMipmapLevel - 1);

	__endSingleTimeCommands(CommandBuffer);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createBuffer(VkDeviceSize vBufferSize, VkBufferUsageFlags vBufferUsage, VkMemoryPropertyFlags vMemoryProperty, VkBuffer& voBuffer, VkDeviceMemory& voBufferDeviceMemory)
{
	VkBufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.size = vBufferSize;
	BufferCreateInfo.usage = vBufferUsage;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(_device(), &BufferCreateInfo, nullptr, &voBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create vertex buffer!");

	VkMemoryRequirements MemoryRequirements = {};
	vkGetBufferMemoryRequirements(_device(), voBuffer, &MemoryRequirements);

	VkMemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = __findMemoryType(MemoryRequirements.memoryTypeBits, vMemoryProperty);

	if (vkAllocateMemory(_device(), &MemoryAllocateInfo, nullptr, &voBufferDeviceMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate memory for vertex buffer!");

	vkBindBufferMemory(_device(), voBuffer, voBufferDeviceMemory, 0);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createVertexBuffer()
{
	VkDeviceSize BufferSize = sizeof(m_VertexData[0]) * m_VertexData.size();

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, m_VertexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pVertexBuffer, m_pVertexBufferDeviceMemory);

	__copyBuffer(pStagingBuffer, m_pVertexBuffer, BufferSize);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createIndexBuffer()
{
	VkDeviceSize BufferSize = sizeof(m_IndexData[0]) * m_IndexData.size();

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, m_IndexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pIndexBuffer, m_pIndexBufferMemory);

	__copyBuffer(pStagingBuffer, m_pIndexBuffer, BufferSize);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createUniformBuffers()
{
	VkDeviceSize BufferSize = sizeof(SUniformBufferObject);

	m_UniformBufferSet.resize(m_SwapChainImageSet.size());
	m_UniformBufferDeviceMemorySet.resize(m_SwapChainImageSet.size());

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		__createBuffer(BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBufferSet[i], m_UniformBufferDeviceMemorySet[i]);
	}
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> DescriptorPoolSizeSet = {};
	DescriptorPoolSizeSet[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorPoolSizeSet[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImageSet.size());
	DescriptorPoolSizeSet[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	DescriptorPoolSizeSet[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImageSet.size());

	VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
	DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	DescriptorPoolCreateInfo.poolSizeCount = 2;
	DescriptorPoolCreateInfo.pPoolSizes = DescriptorPoolSizeSet.data();
	DescriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(m_SwapChainImageSet.size());

	if (vkCreateDescriptorPool(_device(), &DescriptorPoolCreateInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor pool!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> DescriptorSetLayoutSet(m_SwapChainImageSet.size(), m_pDescriptorSetLayout);

	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
	DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorSetAllocateInfo.descriptorPool = m_pDescriptorPool;
	DescriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImageSet.size());
	DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayoutSet.data();

	m_DescriptorSet.resize(m_SwapChainImageSet.size());
	if (vkAllocateDescriptorSets(_device(), &DescriptorSetAllocateInfo, m_DescriptorSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor set!");

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		VkDescriptorBufferInfo DescriptorBufferInfo = {};
		DescriptorBufferInfo.buffer = m_UniformBufferSet[i];
		DescriptorBufferInfo.offset = 0;
		DescriptorBufferInfo.range = sizeof(SUniformBufferObject);

		VkDescriptorImageInfo DescriptorImageInfo = {};
		DescriptorImageInfo.imageView = m_Texture.getImageView();
		DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImageInfo.sampler = m_pTextureSampler;

		std::array<VkWriteDescriptorSet, 2> WriteDescriptors = {};
		WriteDescriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[0].dstSet = m_DescriptorSet[i];
		WriteDescriptors[0].dstBinding = 0;
		WriteDescriptors[0].dstArrayElement = 0;
		WriteDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		WriteDescriptors[0].descriptorCount = 1;
		WriteDescriptors[0].pBufferInfo = &DescriptorBufferInfo;
		WriteDescriptors[0].pImageInfo = nullptr;
		WriteDescriptors[0].pTexelBufferView = nullptr;

		WriteDescriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[1].dstSet = m_DescriptorSet[i];
		WriteDescriptors[1].dstBinding = 1;
		WriteDescriptors[1].dstArrayElement = 0;
		WriteDescriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		WriteDescriptors[1].descriptorCount = 1;
		WriteDescriptors[1].pImageInfo = &DescriptorImageInfo;
		WriteDescriptors[1].pTexelBufferView = nullptr;
		WriteDescriptors[1].pBufferInfo = nullptr;

		vkUpdateDescriptorSets(_device(), static_cast<uint32_t>(WriteDescriptors.size()), WriteDescriptors.data(), 0, nullptr);
	}
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createCommandBuffers()
{
	m_CommandBufferSet.resize(m_FramebufferSet.size());

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = m_pCommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBufferSet.size());

	if (vkAllocateCommandBuffers(_device(), &CommandBufferAllocateInfo, m_CommandBufferSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers!");

	for (auto i = 0; i < m_CommandBufferSet.size(); ++i)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		CommandBufferBeginInfo.pInheritanceInfo = nullptr;

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.renderPass = m_pRenderPass;
		RenderPassBeginInfo.framebuffer = m_FramebufferSet[i];
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.renderArea.extent = _swapchainExtent();

		std::array<VkClearValue, 2> ClearValueSet = {};
		ClearValueSet[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		ClearValueSet[1].depthStencil = { 1.0f,0 };
		RenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(ClearValueSet.size());
		RenderPassBeginInfo.pClearValues = ClearValueSet.data();

		if (vkBeginCommandBuffer(m_CommandBufferSet[i], &CommandBufferBeginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin command buffer!");

		vkCmdBeginRenderPass(m_CommandBufferSet[i], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_CommandBufferSet[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pGraphicsPipeline);
		VkBuffer VertexBuffers[] = { m_pVertexBuffer };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBufferSet[i], 0, 1, VertexBuffers, Offsets);
		vkCmdBindIndexBuffer(m_CommandBufferSet[i], m_pIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(m_CommandBufferSet[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pPipelineLayout, 0, 1, &m_DescriptorSet[i], 0, nullptr);
		vkCmdDrawIndexed(m_CommandBufferSet[i], static_cast<uint32_t>(m_IndexData.size()), 1, 0, 0, 0);
		vkCmdEndRenderPass(m_CommandBufferSet[i]);

		if (vkEndCommandBuffer(m_CommandBufferSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createSyncObjects()
{
	m_ImageAvailableSemaphoreSet.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphoreSet.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFenceSet.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
	SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo FenceCreateInfo = {};
	FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if (vkCreateSemaphore(_device(), &SemaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphoreSet[i]) != VK_SUCCESS || vkCreateSemaphore(_device(), &SemaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphoreSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphores!");

		if (vkCreateFence(_device(), &FenceCreateInfo, nullptr, &m_InFlightFenceSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create fences!");
	}
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__copyBuffer(VkBuffer vSrcBuffer, VkBuffer vDstBuffer, VkDeviceSize vBufferSize)
{
	VkCommandBuffer CommandBuffer = __beginSingleTimeCommands();

	VkBufferCopy CopyRegion = {};
	CopyRegion.size = vBufferSize;
	CopyRegion.srcOffset = 0;
	CopyRegion.dstOffset = 0;
	vkCmdCopyBuffer(CommandBuffer, vSrcBuffer, vDstBuffer, 1, &CopyRegion);

	__endSingleTimeCommands(CommandBuffer);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__updateUniformBuffer(uint32_t vImageIndex)
{
	static auto StartTime = std::chrono::high_resolution_clock::now();

	auto CurrentTime = std::chrono::high_resolution_clock::now();
	float Time = std::chrono::duration<float, std::chrono::seconds::period>(CurrentTime - StartTime).count();

	static float k = 0.0f;
	k += 0.0001;

	SUniformBufferObject UBO = {};
	//UBO.Model = glm::rotate(glm::mat4(1.0f), Time*glm::radians(90.0f), glm::v	ec3(0.0f, 0.0f, 1.0f));
	UBO.View = glm::lookAt(glm::vec3(0.0f + k, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	UBO.Projection = glm::perspective(glm::radians(45.0f), _swapchainExtent().width / static_cast<float>(_swapchainExtent().height), 0.1f, 15.0f);

	UBO.Projection[1][1] *= -1;

	void* Data = nullptr;
	vkMapMemory(_device(), m_UniformBufferDeviceMemorySet[vImageIndex], 0, sizeof(UBO), 0, &Data);
	memcpy(Data, &UBO, sizeof(UBO));
	vkUnmapMemory(_device(), m_UniformBufferDeviceMemorySet[vImageIndex]);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__loadModel()
{
	tinyobj::attrib_t Attribute;
	std::vector<tinyobj::shape_t> ShapeSet;
	std::vector<tinyobj::material_t> MaterialSet;
	std::string Warn, Error;

	if (!tinyobj::LoadObj(&Attribute, &ShapeSet, &MaterialSet, &Warn, &Error, "../../resource/models/cyborg/cyborg.obj"))
		throw std::runtime_error(Warn + Error);

	std::unordered_map<SVertex, uint32_t> UniqueVertices = {};

	for (const auto& Shape : ShapeSet)
	{
		for (const auto& Index : Shape.mesh.indices)
		{
			SVertex Vertex = {};

			Vertex.Position = {
				Attribute.vertices[3 * Index.vertex_index + 0],
				Attribute.vertices[3 * Index.vertex_index + 1],
				Attribute.vertices[3 * Index.vertex_index + 2]
			};

			Vertex.TexCoord = {
				Attribute.texcoords[2 * Index.texcoord_index + 0],
				1.0f - Attribute.texcoords[2 * Index.texcoord_index + 1]
			};

			Vertex.Color = { 1.0f, 1.0f, 1.0f };

			if (UniqueVertices.count(Vertex) == 0)
			{
				UniqueVertices[Vertex] = static_cast<uint32_t>(m_VertexData.size());
				m_VertexData.push_back(Vertex);
			}

			m_IndexData.push_back(UniqueVertices[Vertex]);
		}
	}
}

//************************************************************************************
//Function:
VkImageView VulkanApp::CPerpixelShadingApp::__createImageView(const VkImage& vImage, VkFormat vImageFormat, VkImageAspectFlags vImageAspectFlags, uint32_t vMipmapLevel)
{
	VkImageViewCreateInfo ImageViewCreateInfo = {};
	ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ImageViewCreateInfo.image = vImage;
	ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewCreateInfo.format = vImageFormat;
	ImageViewCreateInfo.subresourceRange.aspectMask = vImageAspectFlags;
	ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.subresourceRange.levelCount = vMipmapLevel;
	ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	ImageViewCreateInfo.subresourceRange.layerCount = 1;
	ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;

	VkImageView ImageView;
	if (vkCreateImageView(_device(), &ImageViewCreateInfo, nullptr, &ImageView) != VK_SUCCESS)
		throw std::runtime_error("Failed to create image view!");

	return ImageView;
}

//************************************************************************************
//Function:
VkFormat VulkanApp::CPerpixelShadingApp::__findSupportedFormat(const std::vector<VkFormat>& vCandidateFormatSet, VkImageTiling vImageTiling, VkFormatFeatureFlags vFormatFeatures)
{
	for (auto Format : vCandidateFormatSet)
	{
		VkFormatProperties FormatProperties;
		vkGetPhysicalDeviceFormatProperties(_physicalDevice(), Format, &FormatProperties);

		if (vImageTiling == VK_IMAGE_TILING_LINEAR && ((FormatProperties.linearTilingFeatures&vFormatFeatures) == vFormatFeatures))
			return Format;

		else if (vImageTiling == VK_IMAGE_TILING_OPTIMAL && ((FormatProperties.optimalTilingFeatures&vFormatFeatures) == vFormatFeatures))
			return Format;
	}

	throw std::runtime_error("Failed to find supported format!");
}

//************************************************************************************
//Function:
uint32_t VulkanApp::CPerpixelShadingApp::__findMemoryType(uint32_t vMemoryTypeFilter, VkMemoryPropertyFlags vMemoryProperty)
{
	VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties = {};
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice(), &PhysicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < PhysicalDeviceMemoryProperties.memoryTypeCount; ++i)
	{
		if ((vMemoryTypeFilter & (1 << i)) && (PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & vMemoryProperty) == vMemoryProperty)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

//************************************************************************************
//Function:
VkSampleCountFlagBits VulkanApp::CPerpixelShadingApp::__getMaxSampleCount()
{
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(_physicalDevice(), &PhysicalDeviceProperties);

	VkSampleCountFlags Counts = std::min(PhysicalDeviceProperties.limits.framebufferColorSampleCounts, PhysicalDeviceProperties.limits.framebufferDepthSampleCounts);
	if (Counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (Counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (Counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (Counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (Counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (Counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

//************************************************************************************
//Function:
VkCommandBuffer VulkanApp::CPerpixelShadingApp::__beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = m_pCommandPool;
	CommandBufferAllocateInfo.commandBufferCount = 1;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	vkAllocateCommandBuffers(_device(), &CommandBufferAllocateInfo, &CommandBuffer);

	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);

	return CommandBuffer;
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__endSingleTimeCommands(VkCommandBuffer vCommandBuffer)
{
	vkEndCommandBuffer(vCommandBuffer);

	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &vCommandBuffer;
	SubmitInfo.signalSemaphoreCount = 0;
	SubmitInfo.pSignalSemaphores = nullptr;
	SubmitInfo.waitSemaphoreCount = 0;
	SubmitInfo.pWaitSemaphores = nullptr;
	SubmitInfo.pWaitDstStageMask = 0;

	vkQueueSubmit(m_pQueue, 1, &SubmitInfo, nullptr);
	vkQueueWaitIdle(m_pQueue);

	vkFreeCommandBuffers(_device(), m_pCommandPool, 1, &vCommandBuffer);
}
