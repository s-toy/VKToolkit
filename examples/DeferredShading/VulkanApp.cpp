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
#include "VkRenderPassCreator.hpp"

//************************************************************************************
//Function:
bool DeferredShading::CDeferredShadingApp::_initV()
{
	if (!hiveVKT::CVkApplicationBase::_initV()) return false;

	__retrieveDeviceQueue();
	__createSwapChain();
	__retrieveSwapChainImagesAndCreateImageViews();
	__createCommandPool();

	__createOffScreenRenderPass();
	__createOffScreenDescriptorSetLayout();
	__createOffScreenPipelineLayout();
	__createDeferredRenderPass();
	__createDeferredDescriptorSetLayout();
	__createDeferredPipelineLayout();
	__createGraphicsPipelines();

	__createOffScreenRenderTargets();
	__createOffScreenFramebuffer();
	__createDeferredFramebuffers();

	__loadModel();
	__createInstanceDataBuffer();
	__createVertexBuffers();
	__createIndexBuffers();

	__createTextureSamplerResources4OffScreenRendering();
	__createTextureSampler4DeferredRendering();

	__createUniformBuffers();

	__createDescriptorPool();
	__createOffScreenDescriptorSet();
	__createDeferredDescriptorSet();

	__createOffScreenCommandBuffers();
	__createDeferredCommandBuffers();

	__createSyncObjects();

	return true;
}

//************************************************************************************
//Function:
bool DeferredShading::CDeferredShadingApp::_renderV()
{
	if (!CVkApplicationBase::_renderV()) return false;

	vkWaitForFences(_device(), 1, &m_InFlightFenceSet[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t ImageIndex = 0;
	VkResult Result = vkAcquireNextImageKHR(_device(), m_pSwapChain, std::numeric_limits<uint64_t>::max(), m_ImageAvailableSemaphoreSet[m_CurrentFrame], VK_NULL_HANDLE, &ImageIndex);
	if (Result != VK_SUCCESS)
		throw std::runtime_error("Failed to acquire swap chain image!");

	__updateUniformBuffer(ImageIndex);

	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore WaitSemaphores_OffScreen[] = { m_ImageAvailableSemaphoreSet[m_CurrentFrame] };
	VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = WaitSemaphores_OffScreen;
	SubmitInfo.pWaitDstStageMask = WaitStages;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &m_OffScreenCommandBufferSet[ImageIndex];
	VkSemaphore SignalSemaphores_OffScreen[] = { m_OffScreenRenderingFinishedSemaphoreSet[m_CurrentFrame] };
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = SignalSemaphores_OffScreen;
	if (vkQueueSubmit(m_pQueue, 1, &SubmitInfo, nullptr) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer!");

	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = SignalSemaphores_OffScreen;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &m_DeferredCommandBufferSet[ImageIndex];
	VkSemaphore SignalSemaphores_Deferred[] = { m_RenderFinishedSemaphoreSet[m_CurrentFrame] };
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = SignalSemaphores_Deferred;
	vkResetFences(_device(), 1, &m_InFlightFenceSet[m_CurrentFrame]);
	if (vkQueueSubmit(m_pQueue, 1, &SubmitInfo, m_InFlightFenceSet[m_CurrentFrame]) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer!");

	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores_Deferred;
	VkSwapchainKHR SwapChains[] = { m_pSwapChain };
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = SwapChains;
	PresentInfo.pImageIndices = &ImageIndex;
	PresentInfo.pResults = nullptr;
	Result = vkQueuePresentKHR(m_pQueue, &PresentInfo);
	if (Result != VK_SUCCESS)
		throw std::runtime_error("Failed to present swap chain image!");

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return true;
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::_destroyV()
{
	_device().waitIdle();

	__cleanup();
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__cleanup()
{
	for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(_device(), m_OffScreenRenderingFinishedSemaphoreSet[i], nullptr);
		vkDestroySemaphore(_device(), m_ImageAvailableSemaphoreSet[i], nullptr);
		vkDestroySemaphore(_device(), m_RenderFinishedSemaphoreSet[i], nullptr);
		vkDestroyFence(_device(), m_InFlightFenceSet[i], nullptr);
	}

	vkDestroyDescriptorPool(_device(), m_pDescriptorPool, nullptr);

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		vkDestroyBuffer(_device(), m_UniformBufferSet_OffScreen[i], nullptr);
		vkFreeMemory(_device(), m_UniformBufferDeviceMemorySet_OffScreen[i], nullptr);
		vkDestroyBuffer(_device(), m_UniformBufferSet_Deferred[i], nullptr);
		vkFreeMemory(_device(), m_UniformBufferDeviceMemorySet_Deferred[i], nullptr);

		vkDestroyImageView(_device(), m_SwapChainImageViewSet[i], nullptr);
		vkDestroyFramebuffer(_device(), m_DeferredFramebufferSet[i], nullptr);
	}

	vkDestroySampler(_device(), m_pSampler4DeferredRendering, nullptr);

	vkDestroySampler(_device(), m_pDiffuseImageSampler, nullptr);
	vkDestroyImageView(_device(), m_pDiffuseImageView, nullptr);
	vkDestroyImage(_device(), m_pDiffuseImage, nullptr);
	vkFreeMemory(_device(), m_pDiffuseImageDeviceMemory, nullptr);
	vkDestroySampler(_device(), m_pSpecularSampler, nullptr);
	vkDestroyImageView(_device(), m_pSpecularImageView, nullptr);
	vkDestroyImage(_device(), m_pSpecularImage, nullptr);
	vkFreeMemory(_device(), m_pSpecularImageDeviceMemory, nullptr);

	vkDestroyBuffer(_device(), m_pVertexBuffer_Model, nullptr);
	vkFreeMemory(_device(), m_pVertexBufferDeviceMemory_Model, nullptr);
	vkDestroyBuffer(_device(), m_pIndexBuffer_Model, nullptr);
	vkFreeMemory(_device(), m_pIndexBufferMemory_Model, nullptr);
	vkDestroyBuffer(_device(), m_pVertexBuffer_Quad, nullptr);
	vkFreeMemory(_device(), m_pVertexBufferDeviceMemory_Quad, nullptr);
	vkDestroyBuffer(_device(), m_pIndexBuffer_Quad, nullptr);
	vkFreeMemory(_device(), m_pIndexBufferMemory_Quad, nullptr);

	vkDestroyBuffer(_device(), m_pInstanceDataBuffer, nullptr);
	vkFreeMemory(_device(), m_pInstanceDataBufferDeviceMemory, nullptr);

	vkDestroyImage(_device(), m_pOffScreenPositionImage, nullptr);
	vkDestroyImageView(_device(), m_pOffScreenPositionImageView, nullptr);
	vkFreeMemory(_device(), m_pOffScreenPositionImageDeviceMemory, nullptr);
	vkDestroyImage(_device(), m_pOffScreenNormalImage, nullptr);
	vkDestroyImageView(_device(), m_pOffScreenNormalImageView, nullptr);
	vkFreeMemory(_device(), m_pOffScreenNormalImageDeviceMemory, nullptr);
	vkDestroyImage(_device(), m_pOffScreenColorImage, nullptr);
	vkDestroyImageView(_device(), m_pOffScreenColorImageView, nullptr);
	vkFreeMemory(_device(), m_pOffScreenColorImageDeviceMemory, nullptr);
	vkDestroyImage(_device(), m_pOffScreenDepthImage, nullptr);
	vkDestroyImageView(_device(), m_pOffScreenDepthImageView, nullptr);
	vkFreeMemory(_device(), m_pOffScreenDepthImageDeviceMemory, nullptr);
	vkDestroyFramebuffer(_device(), m_pOffScreenFramebuffer, nullptr);

	vkDestroyPipeline(_device(), m_pDeferredPipeline, nullptr);
	vkDestroyPipelineLayout(_device(), m_pDeferredPipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(_device(), m_pDeferredDescriptorSetLayout, nullptr);
	vkDestroyRenderPass(_device(), m_pDeferredRenderPass, nullptr);

	vkDestroyPipeline(_device(), m_pOffScreenPipeline, nullptr);
	vkDestroyPipelineLayout(_device(), m_pOffScreenPipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(_device(), m_pOffScreenDescriptorSetLayout, nullptr);
	vkDestroyRenderPass(_device(), m_pOffScreenRenderPass, nullptr);

	vkDestroyCommandPool(_device(), m_pCommandPool, nullptr);

	vkDestroySwapchainKHR(_device(), m_pSwapChain, nullptr);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__retrieveDeviceQueue()
{
	hiveVKT::SQueueFamilyIndices QueueFamilyIndices = _requiredQueueFamilyIndices();

	vkGetDeviceQueue(_device(), QueueFamilyIndices.QueueFamily.value(), 0, &m_pQueue);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createSwapChain()
{
	hiveVKT::SSwapChainSupportDetails SwapChainSupportDetails = _swapChainSupportDetails();
	VkSurfaceFormatKHR SurfaceFormat = __determineSurfaceFormat(SwapChainSupportDetails.SurfaceFormatSet);
	VkPresentModeKHR PresentMode = __determinePresentMode(SwapChainSupportDetails.PresentModeSet);
	VkExtent2D Extent = __determineSwapChainExtent(SwapChainSupportDetails.SurfaceCapabilities);
	uint32_t ImageCount = SwapChainSupportDetails.SurfaceCapabilities.minImageCount + 1;
	if (SwapChainSupportDetails.SurfaceCapabilities.maxImageCount > 0 && ImageCount > SwapChainSupportDetails.SurfaceCapabilities.maxImageCount)
		ImageCount = SwapChainSupportDetails.SurfaceCapabilities.maxImageCount;

	VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.surface = _surface();
	SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
	SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
	SwapchainCreateInfo.imageExtent = Extent;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	SwapchainCreateInfo.queueFamilyIndexCount = 0;
	SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
	SwapchainCreateInfo.minImageCount = ImageCount;
	SwapchainCreateInfo.presentMode = PresentMode;
	SwapchainCreateInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(SwapChainSupportDetails.SurfaceCapabilities.currentTransform);
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(_device(), &SwapchainCreateInfo, nullptr, &m_pSwapChain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swap chain!");

	m_SwapChainImageFormat = SurfaceFormat.format;
	m_SwapChainExtent = Extent;
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__retrieveSwapChainImagesAndCreateImageViews()
{
	uint32_t SwapChainImageCount = 0;
	vkGetSwapchainImagesKHR(_device(), m_pSwapChain, &SwapChainImageCount, nullptr);
	m_SwapChainImageSet.resize(SwapChainImageCount);
	vkGetSwapchainImagesKHR(_device(), m_pSwapChain, &SwapChainImageCount, m_SwapChainImageSet.data());

	m_SwapChainImageViewSet.resize(SwapChainImageCount);

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		m_SwapChainImageViewSet[i] = __createImageView(m_SwapChainImageSet[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createCommandPool()
{
	hiveVKT::SQueueFamilyIndices QueueFamilyIndices = _requiredQueueFamilyIndices();

	VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndices.QueueFamily.value();

	if (vkCreateCommandPool(_device(), &CommandPoolCreateInfo, nullptr, &m_pCommandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenRenderPass()
{
	hiveVKT::CVkRenderPassCreator RenderPassCreator;

	RenderPassCreator.addAttachment(static_cast<vk::Format>(VK_FORMAT_R16G16B16A16_SFLOAT), static_cast<vk::ImageLayout>(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	RenderPassCreator.addAttachment(static_cast<vk::Format>(VK_FORMAT_R16G16B16A16_SFLOAT), static_cast<vk::ImageLayout>(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	RenderPassCreator.addAttachment(static_cast<vk::Format>(VK_FORMAT_R8G8B8A8_UNORM), static_cast<vk::ImageLayout>(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	vk::Format DepthFormat = static_cast<vk::Format>(__findSupportedFormat(gDepthFormatSet, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT));
	RenderPassCreator.addAttachment(DepthFormat, static_cast<vk::ImageLayout>(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));

	std::vector<vk::AttachmentReference> ColorAttachmentReferences;
	ColorAttachmentReferences.push_back({ 0,vk::ImageLayout::eColorAttachmentOptimal });
	ColorAttachmentReferences.push_back({ 1,vk::ImageLayout::eColorAttachmentOptimal });
	ColorAttachmentReferences.push_back({ 2,vk::ImageLayout::eColorAttachmentOptimal });
	vk::AttachmentReference DepthReference = {};
	DepthReference.attachment = 3;
	DepthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	RenderPassCreator.addSubpass(ColorAttachmentReferences, DepthReference, {}, {}, {});

	vk::SubpassDependency SubpassDependency = {};
	SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	SubpassDependency.dstSubpass = 0;
	SubpassDependency.srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	SubpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	SubpassDependency.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
	SubpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	SubpassDependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

	RenderPassCreator.addSubpassDependency(SubpassDependency);

	SubpassDependency.srcSubpass = 0;
	SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	SubpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	SubpassDependency.dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	SubpassDependency.srcAccessMask= vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	SubpassDependency.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
	SubpassDependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;
	
	RenderPassCreator.addSubpassDependency(SubpassDependency);

	m_pOffScreenRenderPass = RenderPassCreator.create(_device());
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenDescriptorSetLayout()
{
	// UBO for Model View and Projection matrixes
	VkDescriptorSetLayoutBinding UniformBufferBinding = {};
	UniformBufferBinding.binding = 0;
	UniformBufferBinding.descriptorCount = 1;
	UniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	UniformBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	UniformBufferBinding.pImmutableSamplers = nullptr;

	// Combined image sampler
	VkDescriptorSetLayoutBinding Diff_SamplerBinding = {};
	Diff_SamplerBinding.binding = 1;
	Diff_SamplerBinding.descriptorCount = 1;
	Diff_SamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	Diff_SamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	Diff_SamplerBinding.pImmutableSamplers = nullptr;

	// Combined image sampler
	VkDescriptorSetLayoutBinding Spec_SamplerBinding = {};
	Spec_SamplerBinding.binding = 2;
	Spec_SamplerBinding.descriptorCount = 1;
	Spec_SamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	Spec_SamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	Spec_SamplerBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 3> Bindings = { UniformBufferBinding,Diff_SamplerBinding,Spec_SamplerBinding };

	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
	DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
	DescriptorSetLayoutCreateInfo.pBindings = Bindings.data();

	if (vkCreateDescriptorSetLayout(_device(), &DescriptorSetLayoutCreateInfo, nullptr, &m_pOffScreenDescriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor set layout!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenPipelineLayout()
{
	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutCreateInfo.setLayoutCount = 1;
	PipelineLayoutCreateInfo.pSetLayouts = &m_pOffScreenDescriptorSetLayout;
	PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(_device(), &PipelineLayoutCreateInfo, nullptr, &m_pOffScreenPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredRenderPass()
{
	VkAttachmentDescription ColorAttachment = {};
	ColorAttachment.format = m_SwapChainImageFormat;
	ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference ColorAttachmentReference = {};
	ColorAttachmentReference.attachment = 0;
	ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription SubpassDescription = {};
	SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	SubpassDescription.colorAttachmentCount = 1;
	SubpassDescription.pColorAttachments = &ColorAttachmentReference;
	SubpassDescription.inputAttachmentCount = 0;
	SubpassDescription.pInputAttachments = nullptr;
	SubpassDescription.preserveAttachmentCount = 0;
	SubpassDescription.pPreserveAttachments = nullptr;
	SubpassDescription.pDepthStencilAttachment = nullptr;
	SubpassDescription.pResolveAttachments = nullptr;

	VkSubpassDependency SubpassDependency = {};
	SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	SubpassDependency.dstSubpass = 0;
	SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	SubpassDependency.srcAccessMask = 0;
	SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo RenderPassCreateInfo = {};
	RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassCreateInfo.attachmentCount = 1;
	RenderPassCreateInfo.pAttachments = &ColorAttachment;
	RenderPassCreateInfo.dependencyCount = 1;
	RenderPassCreateInfo.pDependencies = &SubpassDependency;
	RenderPassCreateInfo.subpassCount = 1;
	RenderPassCreateInfo.pSubpasses = &SubpassDescription;

	if (vkCreateRenderPass(_device(), &RenderPassCreateInfo, nullptr, &m_pDeferredRenderPass) != VK_SUCCESS)
		throw std::runtime_error("Failed to create render pass!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding UniformBufferBinding = {};
	UniformBufferBinding.binding = 0;
	UniformBufferBinding.descriptorCount = 1;
	UniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	UniformBufferBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	UniformBufferBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding Position_SamplerBinding = {};
	Position_SamplerBinding.binding = 1;
	Position_SamplerBinding.descriptorCount = 1;
	Position_SamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	Position_SamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	Position_SamplerBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding Normal_SamplerBinding = {};
	Normal_SamplerBinding.binding = 2;
	Normal_SamplerBinding.descriptorCount = 1;
	Normal_SamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	Normal_SamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	Normal_SamplerBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding Color_SamplerBinding = {};
	Color_SamplerBinding.binding = 3;
	Color_SamplerBinding.descriptorCount = 1;
	Color_SamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	Color_SamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	Color_SamplerBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 4> Bindings = { UniformBufferBinding,Position_SamplerBinding,Normal_SamplerBinding,Color_SamplerBinding };

	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
	DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
	DescriptorSetLayoutCreateInfo.pBindings = Bindings.data();

	if (vkCreateDescriptorSetLayout(_device(), &DescriptorSetLayoutCreateInfo, nullptr, &m_pDeferredDescriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor set layout!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredPipelineLayout()
{
	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutCreateInfo.setLayoutCount = 1;
	PipelineLayoutCreateInfo.pSetLayouts = &m_pDeferredDescriptorSetLayout;
	PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(_device(), &PipelineLayoutCreateInfo, nullptr, &m_pDeferredPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createGraphicsPipelines()
{
	//common states for both pipelines
	VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo = {};
	InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport Viewport = {};
	Viewport.x = 0.0f;
	Viewport.y = 0.0f;
	Viewport.width = static_cast<float>(m_SwapChainExtent.width);
	Viewport.height = static_cast<float>(m_SwapChainExtent.height);
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;

	VkRect2D Scissor = {};
	Scissor.extent = m_SwapChainExtent;
	Scissor.offset = { 0,0 };

	VkPipelineViewportStateCreateInfo ViewportStateCreateInfo = {};
	ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportStateCreateInfo.viewportCount = 1;
	ViewportStateCreateInfo.pViewports = &Viewport;
	ViewportStateCreateInfo.scissorCount = 1;
	ViewportStateCreateInfo.pScissors = &Scissor;

	VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo = {};
	RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	RasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	RasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	RasterizationStateCreateInfo.lineWidth = 1.0f;
	RasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	RasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	RasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	RasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	RasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	RasterizationStateCreateInfo.depthBiasClamp = 0.0f;

	VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo = {};
	MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	MultisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
	MultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	MultisampleStateCreateInfo.minSampleShading = .2f;
	MultisampleStateCreateInfo.pSampleMask = nullptr;
	MultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	MultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo = {};
	DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	DepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	DepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	DepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	DepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	DepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	DepthStencilStateCreateInfo.maxDepthBounds = 1.0f;
	DepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	DepthStencilStateCreateInfo.front = {};
	DepthStencilStateCreateInfo.back = {};

	//create off screen pipeline
	auto VertexShaderCode = ReadFile("OffScreen_vert.spv");
	auto FragmentShaderCode = ReadFile("OffScreen_frag.spv");
	VkShaderModule VertexShaderModule = __createShaderModule(VertexShaderCode);
	VkShaderModule FragmentShaderModule = __createShaderModule(FragmentShaderCode);

	VkPipelineShaderStageCreateInfo VertexShaderStageCreateInfo = {};
	VertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	VertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	VertexShaderStageCreateInfo.module = VertexShaderModule;
	VertexShaderStageCreateInfo.pName = "main";
	VertexShaderStageCreateInfo.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo FragmentShaderStageCreateInfo = {};
	FragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	FragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	FragmentShaderStageCreateInfo.module = FragmentShaderModule;
	FragmentShaderStageCreateInfo.pName = "main";
	FragmentShaderStageCreateInfo.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo ShaderStageCreateInfoSet_OffScreen[] = { VertexShaderStageCreateInfo,FragmentShaderStageCreateInfo };

	auto BindingDescription = SVertex::getBindingDescription();
	auto AttributeDescription = SVertex::getAttributeDescription();
	auto BindingDescription4Instance = SInstanceData::getBindingDescription();
	auto AttributeDescription4Instance = SInstanceData::getAttributeDescription();

	std::vector<VkVertexInputBindingDescription> BindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;
	BindingDescriptions = { BindingDescription , BindingDescription4Instance };
	for (auto i = 0; i < AttributeDescription.size(); ++i)
		AttributeDescriptions.push_back(AttributeDescription[i]);
	for (auto i = 0; i < AttributeDescription4Instance.size(); ++i)
		AttributeDescriptions.push_back(AttributeDescription4Instance[i]);

	VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo = {};
	VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(BindingDescriptions.size());;
	VertexInputStateCreateInfo.pVertexBindingDescriptions = BindingDescriptions.data();
	VertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(AttributeDescriptions.size());
	VertexInputStateCreateInfo.pVertexAttributeDescriptions = AttributeDescriptions.data();

	VkPipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
	ColorBlendAttachmentState.blendEnable = VK_FALSE;
	ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	std::array<VkPipelineColorBlendAttachmentState, 3> ColorBlendAttachmentStates = {
		ColorBlendAttachmentState,
		ColorBlendAttachmentState,
		ColorBlendAttachmentState
	};

	VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo = {};
	ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	ColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(ColorBlendAttachmentStates.size());
	ColorBlendStateCreateInfo.pAttachments = ColorBlendAttachmentStates.data();
	ColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	ColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	ColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	ColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
	GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	GraphicsPipelineCreateInfo.stageCount = 2;
	GraphicsPipelineCreateInfo.pStages = ShaderStageCreateInfoSet_OffScreen;
	GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateCreateInfo;
	GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyStateCreateInfo;
	GraphicsPipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
	GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationStateCreateInfo;
	GraphicsPipelineCreateInfo.pMultisampleState = &MultisampleStateCreateInfo;
	GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilStateCreateInfo;
	GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendStateCreateInfo;
	GraphicsPipelineCreateInfo.pDynamicState = nullptr;
	GraphicsPipelineCreateInfo.layout = m_pOffScreenPipelineLayout;
	GraphicsPipelineCreateInfo.renderPass = m_pOffScreenRenderPass;
	GraphicsPipelineCreateInfo.subpass = 0;
	GraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	GraphicsPipelineCreateInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(_device(), VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, nullptr, &m_pOffScreenPipeline) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline!");

	vkDestroyShaderModule(_device(), VertexShaderModule, nullptr);
	vkDestroyShaderModule(_device(), FragmentShaderModule, nullptr);

	//create deferred shading pipeline
	//RasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;// NOTE 

	VertexShaderCode = ReadFile("Deferred_vert.spv");
	FragmentShaderCode = ReadFile("Deferred_frag.spv");
	VertexShaderModule = __createShaderModule(VertexShaderCode);
	FragmentShaderModule = __createShaderModule(FragmentShaderCode);

	VertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	VertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	VertexShaderStageCreateInfo.module = VertexShaderModule;
	VertexShaderStageCreateInfo.pName = "main";
	VertexShaderStageCreateInfo.pSpecializationInfo = nullptr;

	FragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	FragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	FragmentShaderStageCreateInfo.module = FragmentShaderModule;
	FragmentShaderStageCreateInfo.pName = "main";
	FragmentShaderStageCreateInfo.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo ShaderStageCreateInfoSet_Deferred[] = { VertexShaderStageCreateInfo,FragmentShaderStageCreateInfo };

	auto BindingDescription_Deferred = SQuadVertex::getBindingDescription();
	auto AttributeDescription_Deferred = SQuadVertex::getAttributeDescription();

	VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	VertexInputStateCreateInfo.pVertexBindingDescriptions = &BindingDescription_Deferred;
	VertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(AttributeDescription_Deferred.size());
	VertexInputStateCreateInfo.pVertexAttributeDescriptions = AttributeDescription_Deferred.data();

	ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	ColorBlendStateCreateInfo.attachmentCount = 1;
	ColorBlendStateCreateInfo.pAttachments = &ColorBlendAttachmentState;
	ColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	ColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	ColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	ColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	GraphicsPipelineCreateInfo.stageCount = 2;
	GraphicsPipelineCreateInfo.pStages = ShaderStageCreateInfoSet_Deferred;
	GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateCreateInfo;
	GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendStateCreateInfo;
	GraphicsPipelineCreateInfo.layout = m_pDeferredPipelineLayout;
	GraphicsPipelineCreateInfo.renderPass = m_pDeferredRenderPass;
	GraphicsPipelineCreateInfo.subpass = 0;
	//the rest state is the same with off-screen pipeline

	if (vkCreateGraphicsPipelines(_device(), VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, nullptr, &m_pDeferredPipeline) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline!");

	vkDestroyShaderModule(_device(), VertexShaderModule, nullptr);
	vkDestroyShaderModule(_device(), FragmentShaderModule, nullptr);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenRenderTargets()
{
	__createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pOffScreenPositionImage, m_pOffScreenPositionImageDeviceMemory);
	m_pOffScreenPositionImageView = __createImageView(m_pOffScreenPositionImage, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	__transitionImageLayout(m_pOffScreenPositionImage, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);

	__createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pOffScreenNormalImage, m_pOffScreenNormalImageDeviceMemory);
	m_pOffScreenNormalImageView = __createImageView(m_pOffScreenNormalImage, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	__transitionImageLayout(m_pOffScreenNormalImage, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);

	__createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pOffScreenColorImage, m_pOffScreenColorImageDeviceMemory);
	m_pOffScreenColorImageView = __createImageView(m_pOffScreenColorImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	__transitionImageLayout(m_pOffScreenColorImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);


	VkFormat DepthFormat = __findSupportedFormat(gDepthFormatSet, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	__createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pOffScreenDepthImage, m_pOffScreenDepthImageDeviceMemory);
	m_pOffScreenDepthImageView = __createImageView(m_pOffScreenDepthImage, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	__transitionImageLayout(m_pOffScreenDepthImage, DepthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenFramebuffer()
{
	std::array<VkImageView, 4> Attachments = { m_pOffScreenPositionImageView,m_pOffScreenNormalImageView,m_pOffScreenColorImageView,m_pOffScreenDepthImageView };

	VkFramebufferCreateInfo FramebufferCreateInfo = {};
	FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FramebufferCreateInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
	FramebufferCreateInfo.pAttachments = Attachments.data();
	FramebufferCreateInfo.renderPass = m_pOffScreenRenderPass;
	FramebufferCreateInfo.layers = 1;
	FramebufferCreateInfo.width = m_SwapChainExtent.width;
	FramebufferCreateInfo.height = m_SwapChainExtent.height;

	if (vkCreateFramebuffer(_device(), &FramebufferCreateInfo, nullptr, &m_pOffScreenFramebuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create frame buffer!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredFramebuffers()
{
	m_DeferredFramebufferSet.resize(m_SwapChainImageSet.size());

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		VkFramebufferCreateInfo FramebufferCreateInfo = {};
		FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferCreateInfo.attachmentCount = 1;
		FramebufferCreateInfo.pAttachments = &m_SwapChainImageViewSet[i];
		FramebufferCreateInfo.renderPass = m_pDeferredRenderPass;
		FramebufferCreateInfo.layers = 1;
		FramebufferCreateInfo.width = m_SwapChainExtent.width;
		FramebufferCreateInfo.height = m_SwapChainExtent.height;

		if (vkCreateFramebuffer(_device(), &FramebufferCreateInfo, nullptr, &m_DeferredFramebufferSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create frame buffer!");
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createVertexBuffers()
{
	//create model vertex buffer
	VkDeviceSize BufferSize = sizeof(m_ModelVertexData[0]) * m_ModelVertexData.size();

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, m_ModelVertexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pVertexBuffer_Model, m_pVertexBufferDeviceMemory_Model);

	__copyBuffer(pStagingBuffer, m_pVertexBuffer_Model, BufferSize);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);

	//create quad vertex buffer
	BufferSize = sizeof(gQuadVertexData[0]) * gQuadVertexData.size();

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, gQuadVertexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pVertexBuffer_Quad, m_pVertexBufferDeviceMemory_Quad);

	__copyBuffer(pStagingBuffer, m_pVertexBuffer_Quad, BufferSize);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createIndexBuffers()
{
	//create model index buffer
	VkDeviceSize BufferSize = sizeof(m_ModelIndexData[0]) * m_ModelIndexData.size();

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, m_ModelIndexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pIndexBuffer_Model, m_pIndexBufferMemory_Model);

	__copyBuffer(pStagingBuffer, m_pIndexBuffer_Model, BufferSize);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);

	//create quad index buffer
	BufferSize = sizeof(gQuadIndexData[0]) * gQuadIndexData.size();

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, gQuadIndexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pIndexBuffer_Quad, m_pIndexBufferMemory_Quad);

	__copyBuffer(pStagingBuffer, m_pIndexBuffer_Quad, BufferSize);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createImage(uint32_t vImageWidth, uint32_t vImageHeight, uint32_t vMipmapLevel, VkSampleCountFlagBits vSampleCount, VkFormat vImageFormat, VkImageTiling vImageTiling, VkImageUsageFlags vImageUsages, VkMemoryPropertyFlags vMemoryProperties, VkImage& vImage, VkDeviceMemory& vImageDeviceMemory)
{
	VkImageCreateInfo ImageCreateInfo = {};
	ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	ImageCreateInfo.extent.width = vImageWidth;
	ImageCreateInfo.extent.height = vImageHeight;
	ImageCreateInfo.extent.depth = 1;
	ImageCreateInfo.mipLevels = vMipmapLevel;
	ImageCreateInfo.arrayLayers = 1;
	ImageCreateInfo.format = vImageFormat;
	ImageCreateInfo.tiling = vImageTiling;
	ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ImageCreateInfo.usage = vImageUsages;
	ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ImageCreateInfo.samples = vSampleCount;

	if (vkCreateImage(_device(), &ImageCreateInfo, nullptr, &vImage) != VK_SUCCESS)
		throw std::runtime_error("Failed to create image!");

	VkMemoryRequirements MemoryRequirements = {};
	vkGetImageMemoryRequirements(_device(), vImage, &MemoryRequirements);

	VkMemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = __findMemoryType(MemoryRequirements.memoryTypeBits, vMemoryProperties);

	if (vkAllocateMemory(_device(), &MemoryAllocateInfo, nullptr, &vImageDeviceMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate memory for image!");

	vkBindImageMemory(_device(), vImage, vImageDeviceMemory, 0);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__transitionImageLayout(VkImage vImage, VkFormat vImageFormat, VkImageLayout vOldImageLayout, VkImageLayout vNewImageLayout, uint32_t vMipmapLevel)
{
	VkCommandBuffer CommandBuffer = __beginSingleTimeCommands();

	VkImageMemoryBarrier ImageMemoryBarrier = {};
	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.oldLayout = vOldImageLayout;
	ImageMemoryBarrier.newLayout = vNewImageLayout;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.image = vImage;

	if (vNewImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (vImageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || vImageFormat == VK_FORMAT_D24_UNORM_S8_UINT)
			ImageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else
		ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	ImageMemoryBarrier.subresourceRange.levelCount = vMipmapLevel;
	ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	ImageMemoryBarrier.subresourceRange.layerCount = 1;
	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;

	VkPipelineStageFlags SourceStage;
	VkPipelineStageFlags DestinationStage;

	if (vOldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && vNewImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		ImageMemoryBarrier.srcAccessMask = 0;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (vOldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && vNewImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (vOldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && vNewImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		ImageMemoryBarrier.srcAccessMask = 0;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		DestinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (vOldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && vNewImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		ImageMemoryBarrier.srcAccessMask = 0;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		DestinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else
		throw std::invalid_argument("unsupported layout transition!");

	vkCmdPipelineBarrier(CommandBuffer, SourceStage, DestinationStage, 0, 0, nullptr, 0, nullptr, 1, &ImageMemoryBarrier);

	__endSingleTimeCommands(CommandBuffer);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createTextureSamplerResources4OffScreenRendering()
{
	//diffuse
	int TextureWidth = 0, TextureHeight = 0, TextureChannels = 0;
	unsigned char* Pixels = stbi_load("../../resource/models/cyborg/cyborg_diffuse.png", &TextureWidth, &TextureHeight, &TextureChannels, STBI_rgb_alpha);
	if (!Pixels)
		throw std::runtime_error("Failed to load texture image!");

	uint32_t MipmapLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(TextureWidth, TextureHeight)))) + 1;

	VkDeviceSize ImageSize = TextureWidth * TextureHeight * 4;

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);
	void* Data = nullptr;
	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, ImageSize, 0, &Data);
	memcpy(Data, Pixels, static_cast<size_t>(ImageSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	stbi_image_free(Pixels);

	__createImage(static_cast<uint32_t>(TextureWidth), static_cast<uint32_t>(TextureHeight), MipmapLevel, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pDiffuseImage, m_pDiffuseImageDeviceMemory);
	__transitionImageLayout(m_pDiffuseImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, MipmapLevel);
	__copyBuffer2Image(pStagingBuffer, m_pDiffuseImage, static_cast<uint32_t>(TextureWidth), static_cast<uint32_t>(TextureHeight));
	__generateMipmaps(m_pDiffuseImage, VK_FORMAT_R8G8B8A8_UNORM, TextureWidth, TextureHeight, MipmapLevel);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);

	m_pDiffuseImageView = __createImageView(m_pDiffuseImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, MipmapLevel);

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
	SamplerCreateInfo.maxLod = static_cast<float>(MipmapLevel);
	SamplerCreateInfo.minLod = 0.0f;

	if (vkCreateSampler(_device(), &SamplerCreateInfo, nullptr, &m_pDiffuseImageSampler) != VK_SUCCESS)
		throw std::runtime_error("Failed to create texture sampler!");

	//specular
	Pixels = stbi_load("../../resource/models/cyborg/cyborg_specular.png", &TextureWidth, &TextureHeight, &TextureChannels, STBI_rgb_alpha);
	if (!Pixels)
		throw std::runtime_error("Failed to load texture image!");

	MipmapLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(TextureWidth, TextureHeight)))) + 1;

	ImageSize = TextureWidth * TextureHeight * 4;

	__createBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, ImageSize, 0, &Data);
	memcpy(Data, Pixels, static_cast<size_t>(ImageSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	stbi_image_free(Pixels);

	__createImage(static_cast<uint32_t>(TextureWidth), static_cast<uint32_t>(TextureHeight), MipmapLevel, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pSpecularImage, m_pSpecularImageDeviceMemory);
	__transitionImageLayout(m_pSpecularImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, MipmapLevel);
	__copyBuffer2Image(pStagingBuffer, m_pSpecularImage, static_cast<uint32_t>(TextureWidth), static_cast<uint32_t>(TextureHeight));
	__generateMipmaps(m_pSpecularImage, VK_FORMAT_R8G8B8A8_UNORM, TextureWidth, TextureHeight, MipmapLevel);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);

	m_pSpecularImageView = __createImageView(m_pSpecularImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, MipmapLevel);

	SamplerCreateInfo = {};
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
	SamplerCreateInfo.maxLod = static_cast<float>(MipmapLevel);
	SamplerCreateInfo.minLod = 0.0f;

	if (vkCreateSampler(_device(), &SamplerCreateInfo, nullptr, &m_pSpecularSampler) != VK_SUCCESS)
		throw std::runtime_error("Failed to create texture sampler!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createTextureSampler4DeferredRendering()
{
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
	SamplerCreateInfo.maxLod = 1;
	SamplerCreateInfo.minLod = 0.0f;

	if (vkCreateSampler(_device(), &SamplerCreateInfo, nullptr, &m_pSampler4DeferredRendering) != VK_SUCCESS)
		throw std::runtime_error("Failed to create texture sampler!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__generateMipmaps(VkImage vImage, VkFormat vImageFormat, int32_t vImageWidth, int32_t vImageHeight, uint32_t vMipmapLevel)
{
	VkFormatProperties FormatProperties;
	vkGetPhysicalDeviceFormatProperties(_physicalDevice(), vImageFormat, &FormatProperties);
	if (!(FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		throw std::runtime_error("Texture image format does not support linear blitting!");

	VkCommandBuffer CommandBuffer = __beginSingleTimeCommands();

	VkImageMemoryBarrier ImageMemoryBarrier = {};
	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.image = vImage;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ImageMemoryBarrier.subresourceRange.levelCount = 1;
	ImageMemoryBarrier.subresourceRange.layerCount = 1;
	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;

	int32_t MipmapWidth = vImageWidth;
	int32_t MipmapHeight = vImageHeight;

	for (uint32_t i = 1; i < vMipmapLevel; ++i)
	{
		ImageMemoryBarrier.subresourceRange.baseMipLevel = i - 1;
		ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &ImageMemoryBarrier);

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

		vkCmdBlitImage(CommandBuffer, vImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &ImageBlit, VK_FILTER_LINEAR);

		ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &ImageMemoryBarrier);

		if (MipmapWidth > 1) MipmapWidth /= 2;
		if (MipmapHeight > 1) MipmapHeight /= 2;
	}

	ImageMemoryBarrier.subresourceRange.baseMipLevel = vMipmapLevel - 1;
	ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &ImageMemoryBarrier);

	__endSingleTimeCommands(CommandBuffer);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createBuffer(VkDeviceSize vBufferSize, VkBufferUsageFlags vBufferUsage, VkMemoryPropertyFlags vMemoryProperty, VkBuffer& voBuffer, VkDeviceMemory& voBufferDeviceMemory)
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
void DeferredShading::CDeferredShadingApp::__copyBuffer2Image(VkBuffer vBuffer, VkImage vImage, uint32_t vImageWidth, uint32_t vImageHeight)
{
	VkCommandBuffer CommandBuffer = __beginSingleTimeCommands();

	VkBufferImageCopy CopyRegion = {};
	CopyRegion.bufferOffset = 0;
	CopyRegion.bufferRowLength = 0;
	CopyRegion.bufferImageHeight = 0;
	CopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	CopyRegion.imageSubresource.mipLevel = 0;
	CopyRegion.imageSubresource.layerCount = 1;
	CopyRegion.imageSubresource.baseArrayLayer = 0;
	CopyRegion.imageExtent = { vImageWidth,vImageHeight,1 };
	CopyRegion.imageOffset = { 0,0,0 };

	vkCmdCopyBufferToImage(CommandBuffer, vBuffer, vImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &CopyRegion);

	__endSingleTimeCommands(CommandBuffer);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createUniformBuffers()
{
	VkDeviceSize BufferSize_OffScreen = sizeof(SUniformBufferObject_OffScreen);
	VkDeviceSize BufferSize_Deferred = sizeof(SUniformBufferObject_Deferred);

	m_UniformBufferSet_OffScreen.resize(m_SwapChainImageSet.size());
	m_UniformBufferDeviceMemorySet_OffScreen.resize(m_SwapChainImageSet.size());
	m_UniformBufferSet_Deferred.resize(m_SwapChainImageSet.size());
	m_UniformBufferDeviceMemorySet_Deferred.resize(m_SwapChainImageSet.size());

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		__createBuffer(BufferSize_OffScreen, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBufferSet_OffScreen[i], m_UniformBufferDeviceMemorySet_OffScreen[i]);
		__createBuffer(BufferSize_Deferred, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBufferSet_Deferred[i], m_UniformBufferDeviceMemorySet_Deferred[i]);
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDescriptorPool()
{
	uint32_t NumImageInSwapChain = static_cast<uint32_t>(m_SwapChainImageSet.size());
	uint32_t NumUBO = NumImageInSwapChain * (1 + 1);
	uint32_t NumSampler = NumImageInSwapChain * (2 + 3);
	uint32_t NumSet = NumImageInSwapChain * 2;

	std::array<VkDescriptorPoolSize, 2> DescriptorPoolSizeSet = {};
	DescriptorPoolSizeSet[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorPoolSizeSet[0].descriptorCount = NumUBO;
	DescriptorPoolSizeSet[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	DescriptorPoolSizeSet[1].descriptorCount = NumSampler;

	VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
	DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	DescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(DescriptorPoolSizeSet.size());
	DescriptorPoolCreateInfo.pPoolSizes = DescriptorPoolSizeSet.data();
	DescriptorPoolCreateInfo.maxSets = NumSet;

	if (vkCreateDescriptorPool(_device(), &DescriptorPoolCreateInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor pool!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> DescriptorSetLayoutSet(m_SwapChainImageSet.size(), m_pOffScreenDescriptorSetLayout);

	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
	DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorSetAllocateInfo.descriptorPool = m_pDescriptorPool;
	DescriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImageSet.size());
	DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayoutSet.data();

	m_OffScreenDescriptorSet.resize(m_SwapChainImageSet.size());
	if (vkAllocateDescriptorSets(_device(), &DescriptorSetAllocateInfo, m_OffScreenDescriptorSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor set!");

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		//model, view and projection 
		VkDescriptorBufferInfo DescriptorBufferInfo = {};
		DescriptorBufferInfo.buffer = m_UniformBufferSet_OffScreen[i];
		DescriptorBufferInfo.offset = 0;
		DescriptorBufferInfo.range = sizeof(SUniformBufferObject_OffScreen);

		VkDescriptorImageInfo DescriptorImageInfo_Diffuse = {};
		DescriptorImageInfo_Diffuse.imageView = m_pDiffuseImageView;
		DescriptorImageInfo_Diffuse.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImageInfo_Diffuse.sampler = m_pDiffuseImageSampler;

		VkDescriptorImageInfo DescriptorImageInfo_Specular = {};
		DescriptorImageInfo_Specular.imageView = m_pSpecularImageView;
		DescriptorImageInfo_Specular.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImageInfo_Specular.sampler = m_pSpecularSampler;

		std::array<VkWriteDescriptorSet, 3> WriteDescriptors = {};
		WriteDescriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[0].dstSet = m_OffScreenDescriptorSet[i];
		WriteDescriptors[0].dstBinding = 0;
		WriteDescriptors[0].dstArrayElement = 0;
		WriteDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		WriteDescriptors[0].descriptorCount = 1;
		WriteDescriptors[0].pBufferInfo = &DescriptorBufferInfo;
		WriteDescriptors[0].pImageInfo = nullptr;
		WriteDescriptors[0].pTexelBufferView = nullptr;

		WriteDescriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[1].dstSet = m_OffScreenDescriptorSet[i];
		WriteDescriptors[1].dstBinding = 1;
		WriteDescriptors[1].dstArrayElement = 0;
		WriteDescriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		WriteDescriptors[1].descriptorCount = 1;
		WriteDescriptors[1].pImageInfo = &DescriptorImageInfo_Diffuse;
		WriteDescriptors[1].pTexelBufferView = nullptr;
		WriteDescriptors[1].pBufferInfo = nullptr;

		WriteDescriptors[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[2].dstSet = m_OffScreenDescriptorSet[i];
		WriteDescriptors[2].dstBinding = 2;
		WriteDescriptors[2].dstArrayElement = 0;
		WriteDescriptors[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		WriteDescriptors[2].descriptorCount = 1;
		WriteDescriptors[2].pImageInfo = &DescriptorImageInfo_Specular;
		WriteDescriptors[2].pTexelBufferView = nullptr;
		WriteDescriptors[2].pBufferInfo = nullptr;

		vkUpdateDescriptorSets(_device(), static_cast<uint32_t>(WriteDescriptors.size()), WriteDescriptors.data(), 0, nullptr);
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> DescriptorSetLayoutSet(m_SwapChainImageSet.size(), m_pDeferredDescriptorSetLayout);

	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
	DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorSetAllocateInfo.descriptorPool = m_pDescriptorPool;
	DescriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImageSet.size());
	DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayoutSet.data();

	m_DeferredDescriptorSet.resize(m_SwapChainImageSet.size());
	if (vkAllocateDescriptorSets(_device(), &DescriptorSetAllocateInfo, m_DeferredDescriptorSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor set!");

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		//view position
		VkDescriptorBufferInfo DescriptorBufferInfo = {};
		DescriptorBufferInfo.buffer = m_UniformBufferSet_OffScreen[i];
		DescriptorBufferInfo.offset = 0;
		DescriptorBufferInfo.range = sizeof(SUniformBufferObject_Deferred);

		VkDescriptorImageInfo DescriptorImageInfo_Position = {};
		DescriptorImageInfo_Position.imageView = m_pOffScreenPositionImageView;
		DescriptorImageInfo_Position.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImageInfo_Position.sampler = m_pSampler4DeferredRendering;

		VkDescriptorImageInfo DescriptorImageInfo_Normal = {};
		DescriptorImageInfo_Normal.imageView = m_pOffScreenNormalImageView;
		DescriptorImageInfo_Normal.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImageInfo_Normal.sampler = m_pSampler4DeferredRendering;

		VkDescriptorImageInfo DescriptorImageInfo_Color = {};
		DescriptorImageInfo_Color.imageView = m_pOffScreenColorImageView;
		DescriptorImageInfo_Color.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImageInfo_Color.sampler = m_pSampler4DeferredRendering;

		std::array<VkWriteDescriptorSet, 4> WriteDescriptors = {};
		WriteDescriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[0].dstSet = m_DeferredDescriptorSet[i];
		WriteDescriptors[0].dstBinding = 0;
		WriteDescriptors[0].dstArrayElement = 0;
		WriteDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		WriteDescriptors[0].descriptorCount = 1;
		WriteDescriptors[0].pBufferInfo = &DescriptorBufferInfo;
		WriteDescriptors[0].pImageInfo = nullptr;
		WriteDescriptors[0].pTexelBufferView = nullptr;

		WriteDescriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[1].dstSet = m_DeferredDescriptorSet[i];
		WriteDescriptors[1].dstBinding = 1;
		WriteDescriptors[1].dstArrayElement = 0;
		WriteDescriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		WriteDescriptors[1].descriptorCount = 1;
		WriteDescriptors[1].pImageInfo = &DescriptorImageInfo_Position;
		WriteDescriptors[1].pTexelBufferView = nullptr;
		WriteDescriptors[1].pBufferInfo = nullptr;

		WriteDescriptors[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[2].dstSet = m_DeferredDescriptorSet[i];
		WriteDescriptors[2].dstBinding = 2;
		WriteDescriptors[2].dstArrayElement = 0;
		WriteDescriptors[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		WriteDescriptors[2].descriptorCount = 1;
		WriteDescriptors[2].pImageInfo = &DescriptorImageInfo_Normal;
		WriteDescriptors[2].pTexelBufferView = nullptr;
		WriteDescriptors[2].pBufferInfo = nullptr;

		WriteDescriptors[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[3].dstSet = m_DeferredDescriptorSet[i];
		WriteDescriptors[3].dstBinding = 3;
		WriteDescriptors[3].dstArrayElement = 0;
		WriteDescriptors[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		WriteDescriptors[3].descriptorCount = 1;
		WriteDescriptors[3].pImageInfo = &DescriptorImageInfo_Color;
		WriteDescriptors[3].pTexelBufferView = nullptr;
		WriteDescriptors[3].pBufferInfo = nullptr;

		vkUpdateDescriptorSets(_device(), static_cast<uint32_t>(WriteDescriptors.size()), WriteDescriptors.data(), 0, nullptr);
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenCommandBuffers()
{
	m_OffScreenCommandBufferSet.resize(m_SwapChainImageSet.size());

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = m_pCommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_OffScreenCommandBufferSet.size());

	if (vkAllocateCommandBuffers(_device(), &CommandBufferAllocateInfo, m_OffScreenCommandBufferSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers!");

	for (auto i = 0; i < m_OffScreenCommandBufferSet.size(); ++i)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		CommandBufferBeginInfo.pInheritanceInfo = nullptr;

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.renderPass = m_pOffScreenRenderPass;
		RenderPassBeginInfo.framebuffer = m_pOffScreenFramebuffer;
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.renderArea.extent = m_SwapChainExtent;

		std::array<VkClearValue, 4> ClearValueSet = {};
		ClearValueSet[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		ClearValueSet[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		ClearValueSet[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		ClearValueSet[3].depthStencil = { 1.0f,0 };
		RenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(ClearValueSet.size());
		RenderPassBeginInfo.pClearValues = ClearValueSet.data();

		if (vkBeginCommandBuffer(m_OffScreenCommandBufferSet[i], &CommandBufferBeginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin command buffer!");

		vkCmdBeginRenderPass(m_OffScreenCommandBufferSet[i], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_OffScreenCommandBufferSet[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pOffScreenPipeline);
		VkBuffer VertexBuffers[] = { m_pVertexBuffer_Model };
		VkBuffer InstanceBuffers[] = { m_pInstanceDataBuffer };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_OffScreenCommandBufferSet[i], 0, 1, VertexBuffers, Offsets);
		vkCmdBindVertexBuffers(m_OffScreenCommandBufferSet[i], 1, 1, InstanceBuffers, Offsets);
		vkCmdBindIndexBuffer(m_OffScreenCommandBufferSet[i], m_pIndexBuffer_Model, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(m_OffScreenCommandBufferSet[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pOffScreenPipelineLayout, 0, 1, &m_OffScreenDescriptorSet[i], 0, nullptr);
		vkCmdDrawIndexed(m_OffScreenCommandBufferSet[i], static_cast<uint32_t>(m_ModelIndexData.size()), 4, 0, 0, 0);
		vkCmdEndRenderPass(m_OffScreenCommandBufferSet[i]);

		if (vkEndCommandBuffer(m_OffScreenCommandBufferSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredCommandBuffers()
{
	m_DeferredCommandBufferSet.resize(m_SwapChainImageSet.size());

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = m_pCommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_DeferredCommandBufferSet.size());

	if (vkAllocateCommandBuffers(_device(), &CommandBufferAllocateInfo, m_DeferredCommandBufferSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers!");

	for (auto i = 0; i < m_DeferredCommandBufferSet.size(); ++i)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		CommandBufferBeginInfo.pInheritanceInfo = nullptr;

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.renderPass = m_pDeferredRenderPass;
		RenderPassBeginInfo.framebuffer = m_DeferredFramebufferSet[i];
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.renderArea.extent = m_SwapChainExtent;

		std::array<VkClearValue, 1> ClearValueSet = {};
		ClearValueSet[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		RenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(ClearValueSet.size());
		RenderPassBeginInfo.pClearValues = ClearValueSet.data();

		if (vkBeginCommandBuffer(m_DeferredCommandBufferSet[i], &CommandBufferBeginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin command buffer!");

		vkCmdBeginRenderPass(m_DeferredCommandBufferSet[i], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_DeferredCommandBufferSet[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pDeferredPipeline);
		VkBuffer VertexBuffers[] = { m_pVertexBuffer_Quad };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_DeferredCommandBufferSet[i], 0, 1, VertexBuffers, Offsets);
		vkCmdBindIndexBuffer(m_DeferredCommandBufferSet[i], m_pIndexBuffer_Quad, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(m_DeferredCommandBufferSet[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pDeferredPipelineLayout, 0, 1, &m_DeferredDescriptorSet[i], 0, nullptr);
		vkCmdDrawIndexed(m_DeferredCommandBufferSet[i], static_cast<uint32_t>(gQuadIndexData.size()), 1, 0, 0, 0);
		vkCmdEndRenderPass(m_DeferredCommandBufferSet[i]);

		if (vkEndCommandBuffer(m_DeferredCommandBufferSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createSyncObjects()
{
	m_OffScreenRenderingFinishedSemaphoreSet.resize(MAX_FRAMES_IN_FLIGHT);
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
		if (vkCreateSemaphore(_device(), &SemaphoreCreateInfo, nullptr, &m_OffScreenRenderingFinishedSemaphoreSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphores!");

		if (vkCreateSemaphore(_device(), &SemaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphoreSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphores!");

		if (vkCreateSemaphore(_device(), &SemaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphoreSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphores!");

		if (vkCreateFence(_device(), &FenceCreateInfo, nullptr, &m_InFlightFenceSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create fences!");
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__copyBuffer(VkBuffer vSrcBuffer, VkBuffer vDstBuffer, VkDeviceSize vBufferSize)
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
void DeferredShading::CDeferredShadingApp::__updateUniformBuffer(uint32_t vImageIndex)
{
	glm::vec3 ViewPosition = glm::vec3(0.0f, 3.0f, 3.0f);

	//off-screen
	SUniformBufferObject_OffScreen UBO = {};

	UBO.Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.45f, 0.45f, 0.45f));
	UBO.View = glm::lookAt(ViewPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	UBO.Projection = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / (float)m_SwapChainExtent.height, 0.1f, 10.0f);

	UBO.Projection[1][1] *= -1;

	void* Data = nullptr;
	vkMapMemory(_device(), m_UniformBufferDeviceMemorySet_OffScreen[vImageIndex], 0, sizeof(UBO), 0, &Data);
	memcpy(Data, &UBO, sizeof(UBO));
	vkUnmapMemory(_device(), m_UniformBufferDeviceMemorySet_OffScreen[vImageIndex]);

	//deferred
	SUniformBufferObject_Deferred UBO_Deferred = {};
	UBO_Deferred.ViewPosition = ViewPosition;

	vkMapMemory(_device(), m_UniformBufferDeviceMemorySet_Deferred[vImageIndex], 0, sizeof(UBO_Deferred), 0, &Data);
	memcpy(Data, &UBO_Deferred, sizeof(UBO_Deferred));
	vkUnmapMemory(_device(), m_UniformBufferDeviceMemorySet_Deferred[vImageIndex]);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__loadModel()
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

			Vertex.Normal = {
				Attribute.normals[3 * Index.normal_index + 0],
				Attribute.normals[3 * Index.normal_index + 1],
				Attribute.normals[3 * Index.normal_index + 2]
			};

			if (UniqueVertices.count(Vertex) == 0)
			{
				UniqueVertices[Vertex] = static_cast<uint32_t>(m_ModelVertexData.size());
				m_ModelVertexData.push_back(Vertex);
			}

			m_ModelIndexData.push_back(UniqueVertices[Vertex]);
		}
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createInstanceDataBuffer()
{
	VkDeviceSize BufferSize = sizeof(gInstanceData4Model[0])* gInstanceData4Model.size();

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(_device(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, gInstanceData4Model.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(_device(), pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pInstanceDataBuffer, m_pInstanceDataBufferDeviceMemory);

	__copyBuffer(pStagingBuffer, m_pInstanceDataBuffer, BufferSize);

	vkDestroyBuffer(_device(), pStagingBuffer, nullptr);
	vkFreeMemory(_device(), pStagingBufferDeviceMemory, nullptr);
}

//************************************************************************************
//Function:
VkSurfaceFormatKHR DeferredShading::CDeferredShadingApp::__determineSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& vCandidateSurfaceFormatSet)const
{
	if (vCandidateSurfaceFormatSet.size() == 1 && vCandidateSurfaceFormatSet[0].format == vk::Format::eUndefined)
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	for (const auto& Format : vCandidateSurfaceFormatSet)
	{
		if (Format.format == vk::Format::eB8G8R8A8Unorm && Format.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
			return Format;
	}
	return vCandidateSurfaceFormatSet[0];
}

//************************************************************************************
//Function:
VkPresentModeKHR DeferredShading::CDeferredShadingApp::__determinePresentMode(const std::vector<vk::PresentModeKHR>& vCandidatePresentModeSet)const
{
	VkPresentModeKHR BestPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& PresentMode : vCandidatePresentModeSet)
	{
		if (static_cast<VkPresentModeKHR>(PresentMode) == VK_PRESENT_MODE_MAILBOX_KHR)
			return static_cast<VkPresentModeKHR>(PresentMode);
		else if (static_cast<VkPresentModeKHR>(PresentMode) == VK_PRESENT_MODE_IMMEDIATE_KHR)
			BestPresentMode = static_cast<VkPresentModeKHR>(PresentMode);
	}

	return BestPresentMode;
}

//************************************************************************************
//Function:
VkExtent2D DeferredShading::CDeferredShadingApp::__determineSwapChainExtent(const VkSurfaceCapabilitiesKHR& vSurfaceCapabilities)const
{
	if (vSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return vSurfaceCapabilities.currentExtent;
	}
	else
	{
		int WindowWidth = 0, WindowHeight = 0;
		//glfwGetFramebufferSize(m_pGLFWWindow, &WindowWidth, &WindowHeight);

		VkExtent2D ActualExtent = { static_cast<uint32_t>(WindowWidth), static_cast<uint32_t>(WindowHeight) };

		ActualExtent.width = std::max(vSurfaceCapabilities.minImageExtent.width, std::min(vSurfaceCapabilities.maxImageExtent.width, ActualExtent.width));
		ActualExtent.height = std::max(vSurfaceCapabilities.minImageExtent.height, std::min(vSurfaceCapabilities.maxImageExtent.height, ActualExtent.height));

		return ActualExtent;
	}
}

//************************************************************************************
//Function:
VkImageView DeferredShading::CDeferredShadingApp::__createImageView(const VkImage& vImage, VkFormat vImageFormat, VkImageAspectFlags vImageAspectFlags, uint32_t vMipmapLevel)
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
VkFormat DeferredShading::CDeferredShadingApp::__findSupportedFormat(const std::vector<VkFormat>& vCandidateFormatSet, VkImageTiling vImageTiling, VkFormatFeatureFlags vFormatFeatures)
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
VkShaderModule DeferredShading::CDeferredShadingApp::__createShaderModule(const std::vector<char>& vShaderCode)
{
	VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
	ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderModuleCreateInfo.codeSize = vShaderCode.size();
	ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vShaderCode.data());

	VkShaderModule ShaderModule;
	if (vkCreateShaderModule(_device(), &ShaderModuleCreateInfo, nullptr, &ShaderModule) != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module!");

	return ShaderModule;
}

//************************************************************************************
//Function:
uint32_t DeferredShading::CDeferredShadingApp::__findMemoryType(uint32_t vMemoryTypeFilter, VkMemoryPropertyFlags vMemoryProperty)
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
VkCommandBuffer DeferredShading::CDeferredShadingApp::__beginSingleTimeCommands()
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
void DeferredShading::CDeferredShadingApp::__endSingleTimeCommands(VkCommandBuffer vCommandBuffer)
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

//************************************************************************************
//Function:
VKAPI_ATTR VkBool32 VKAPI_CALL DeferredShading::CDeferredShadingApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, VkDebugUtilsMessageTypeFlagsEXT vMessageTypeFlags, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (vMessageSeverityFlags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		std::cout << "Validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}
