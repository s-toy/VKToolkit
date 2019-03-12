#include "VulkanApp.h"
#include <set>
#include <iostream>
#include <chrono>
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/gtc/matrix_transform.hpp>
#include "VkGraphicsPipelineCreator.hpp"
#include "VkShaderModuleCreator.hpp"
#include "VkInstanceCreator.hpp"
#include "VkDeviceCreator.hpp"

using namespace hiveVKT;

//************************************************************************************
//Function:
bool VulkanApp::CPerpixelShadingApp::_initV()
{
	if (!CVkApplicationBase::_initV()) return false;

	m_pGLFWWindow = this->_getGLFWwindow();
	_ASSERTE(m_pGLFWWindow);

	__prepareLayersAndExtensions();

	hiveVKT::CVkInstanceCreator InstanceCreator;
	m_Instance = InstanceCreator.create();

	m_DebugMessenger.setupDebugMessenger(m_Instance);

	__createSurface();
	__pickPhysicalDevice();
	__createDevice();
	__retrieveDeviceQueue();
	__createSwapChain();
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

	vkWaitForFences(m_Device, 1, &m_InFlightFenceSet[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t ImageIndex = 0;
	VkResult Result = vkAcquireNextImageKHR(m_Device, m_pSwapChain, std::numeric_limits<uint64_t>::max(), m_ImageAvailableSemaphoreSet[m_CurrentFrame], VK_NULL_HANDLE, &ImageIndex);
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
	vkResetFences(m_Device, 1, &m_InFlightFenceSet[m_CurrentFrame]);
	if (vkQueueSubmit(m_pQueue, 1, &SubmitInfo, m_InFlightFenceSet[m_CurrentFrame]) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer!");

	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores;
	VkSwapchainKHR SwapChains[] = { m_pSwapChain };
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
	CVkApplicationBase::_destroyV();

	m_Device.waitIdle();

	for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphoreSet[i], nullptr);
		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphoreSet[i], nullptr);
		vkDestroyFence(m_Device, m_InFlightFenceSet[i], nullptr);
	}

	vkDestroyDescriptorPool(m_Device, m_pDescriptorPool, nullptr);

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		vkDestroyBuffer(m_Device, m_UniformBufferSet[i], nullptr);
		vkFreeMemory(m_Device, m_UniformBufferDeviceMemorySet[i], nullptr);
	}

	vkDestroyBuffer(m_Device, m_pIndexBuffer, nullptr);
	vkFreeMemory(m_Device, m_pIndexBufferMemory, nullptr);
	vkDestroyBuffer(m_Device, m_pVertexBuffer, nullptr);
	vkFreeMemory(m_Device, m_pVertexBufferDeviceMemory, nullptr);

	vkDestroySampler(m_Device, m_pTextureSampler, nullptr);
	vkDestroyImageView(m_Device, m_pTextureImageView, nullptr);
	vkDestroyImage(m_Device, m_pTextureImage, nullptr);
	vkFreeMemory(m_Device, m_pTextureImageDeviceMemory, nullptr);

	for (auto i = 0; i < m_FramebufferSet.size(); ++i)
		vkDestroyFramebuffer(m_Device, m_FramebufferSet[i], nullptr);

	vkDestroyImageView(m_Device, m_pDepthImageView, nullptr);
	vkDestroyImage(m_Device, m_pDepthImage, nullptr);
	vkFreeMemory(m_Device, m_pDepthImageDeviceMemory, nullptr);

	vkDestroyImageView(m_Device, m_pMsaaImageView, nullptr);
	vkDestroyImage(m_Device, m_pMsaaImage, nullptr);
	vkFreeMemory(m_Device, m_pMsaaImageDeviceMemory, nullptr);

	vkDestroyCommandPool(m_Device, m_pCommandPool, nullptr);

	vkDestroyPipeline(m_Device, m_pGraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_Device, m_pPipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(m_Device, m_pDescriptorSetLayout, nullptr);
	vkDestroyRenderPass(m_Device, m_pRenderPass, nullptr);

	for (auto i = 0; i < m_SwapChainImageViewSet.size(); ++i)
		vkDestroyImageView(m_Device, m_SwapChainImageViewSet[i], nullptr);
	vkDestroySwapchainKHR(m_Device, m_pSwapChain, nullptr);

	vkDestroyDevice(m_Device, nullptr);
	vkDestroySurfaceKHR(m_Instance, m_pSurface, nullptr);

	m_DebugMessenger.destroyDebugMessenger(m_Instance);

	vkDestroyInstance(m_Instance, nullptr);

	glfwDestroyWindow(m_pGLFWWindow);
	glfwTerminate();
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__prepareLayersAndExtensions()
{
	if (g_enableValidationLayers)
	{
		m_enabledLayersAtDeviceLevel.emplace_back("VK_LAYER_LUNARG_standard_validation");
	}

	m_enabledExtensionsAtDeviceLevel.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createSurface()
{
	if (glfwCreateWindowSurface(m_Instance, m_pGLFWWindow, nullptr, &m_pSurface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__pickPhysicalDevice()
{
	uint32_t PhysicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &PhysicalDeviceCount, nullptr);
	std::vector<VkPhysicalDevice> PhysicalDeviceSet(PhysicalDeviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &PhysicalDeviceCount, PhysicalDeviceSet.data());

	for (auto PhysicalDevice : PhysicalDeviceSet)
	{
		if (__isPhysicalDeviceSuitable(PhysicalDevice))
		{
			m_pPhysicalDevice = PhysicalDevice;
			m_SampleCount = __getMaxSampleCount();
			break;
		}
	}

	if (m_pPhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find a suitable GPU!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createDevice()
{
	hiveVKT::CVkDeviceCreator DeviceCreator;

	SQueueFamilyIndices QueueFamilyIndices = __findRequiredQueueFamilies(m_pPhysicalDevice);
	DeviceCreator.addQueue(QueueFamilyIndices.QueueFamily.value(), 1, 1.0f);

	vk::PhysicalDeviceFeatures PhysicalDeviceFeatures;
	PhysicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	PhysicalDeviceFeatures.sampleRateShading = VK_TRUE;
	DeviceCreator.setPhysicalDeviceFeatures(&PhysicalDeviceFeatures);

	m_Device = DeviceCreator.create(m_pPhysicalDevice);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__retrieveDeviceQueue()
{
	SQueueFamilyIndices QueueFamilyIndices = __findRequiredQueueFamilies(m_pPhysicalDevice);

	vkGetDeviceQueue(m_Device, QueueFamilyIndices.QueueFamily.value(), 0, &m_pQueue);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createSwapChain()
{
	SSwapChainSupportDetails SwapChainSupportDetails = __queryPhysicalDeviceSwapChainSupport(m_pPhysicalDevice);
	VkSurfaceFormatKHR SurfaceFormat = __determineSurfaceFormat(SwapChainSupportDetails.SurfaceFormatSet);
	VkPresentModeKHR PresentMode = __determinePresentMode(SwapChainSupportDetails.PresentModeSet);
	VkExtent2D Extent = __determineSwapChainExtent(SwapChainSupportDetails.SurfaceCapabilities);
	uint32_t ImageCount = SwapChainSupportDetails.SurfaceCapabilities.minImageCount + 1;
	if (SwapChainSupportDetails.SurfaceCapabilities.maxImageCount > 0 && ImageCount > SwapChainSupportDetails.SurfaceCapabilities.maxImageCount)
		ImageCount = SwapChainSupportDetails.SurfaceCapabilities.maxImageCount;

	VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.surface = m_pSurface;
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
	SwapchainCreateInfo.preTransform = SwapChainSupportDetails.SurfaceCapabilities.currentTransform;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_Device, &SwapchainCreateInfo, nullptr, &m_pSwapChain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swap chain!");

	m_SwapChainImageFormat = SurfaceFormat.format;
	m_SwapChainExtent = Extent;
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__retrieveSwapChainImagesAndCreateImageViews()
{
	uint32_t SwapChainImageCount = 0;
	vkGetSwapchainImagesKHR(m_Device, m_pSwapChain, &SwapChainImageCount, nullptr);
	m_SwapChainImageSet.resize(SwapChainImageCount);
	vkGetSwapchainImagesKHR(m_Device, m_pSwapChain, &SwapChainImageCount, m_SwapChainImageSet.data());

	m_SwapChainImageViewSet.resize(SwapChainImageCount);

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		m_SwapChainImageViewSet[i] = __createImageView(m_SwapChainImageSet[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createRenderPass()
{
	VkAttachmentDescription ColorAttachment = {};
	ColorAttachment.format = m_SwapChainImageFormat;
	ColorAttachment.samples = m_SampleCount;
	ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription DepthAttachment = {};
	DepthAttachment.format = __findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	DepthAttachment.samples = m_SampleCount;
	DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription ColorAttachmentResolve = {};
	ColorAttachmentResolve.format = m_SwapChainImageFormat;
	ColorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	ColorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	ColorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	ColorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	ColorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ColorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	std::array<VkAttachmentDescription, 3> Attachments = { ColorAttachment,DepthAttachment,ColorAttachmentResolve };

	VkAttachmentReference ColorAttachmentReference = {};
	ColorAttachmentReference.attachment = 0;
	ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference DepthAttachmentReference = {};
	DepthAttachmentReference.attachment = 1;
	DepthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference ColorAttachmentResolveReference = {};
	ColorAttachmentResolveReference.attachment = 2;
	ColorAttachmentResolveReference.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkSubpassDescription SubpassDescription = {};
	SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	SubpassDescription.colorAttachmentCount = 1;
	SubpassDescription.pColorAttachments = &ColorAttachmentReference;
	SubpassDescription.inputAttachmentCount = 0;
	SubpassDescription.pInputAttachments = nullptr;
	SubpassDescription.preserveAttachmentCount = 0;
	SubpassDescription.pPreserveAttachments = nullptr;
	SubpassDescription.pDepthStencilAttachment = &DepthAttachmentReference;
	SubpassDescription.pResolveAttachments = &ColorAttachmentResolveReference;

	VkSubpassDependency SubpassDependency = {};
	SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	SubpassDependency.dstSubpass = 0;
	SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	SubpassDependency.srcAccessMask = 0;
	SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo RenderPassCreateInfo = {};
	RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassCreateInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
	RenderPassCreateInfo.pAttachments = Attachments.data();
	RenderPassCreateInfo.dependencyCount = 1;
	RenderPassCreateInfo.pDependencies = &SubpassDependency;
	RenderPassCreateInfo.subpassCount = 1;
	RenderPassCreateInfo.pSubpasses = &SubpassDescription;

	if (vkCreateRenderPass(m_Device, &RenderPassCreateInfo, nullptr, &m_pRenderPass) != VK_SUCCESS)
		throw std::runtime_error("Failed to create render pass!");
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

	if (vkCreateDescriptorSetLayout(m_Device, &DescriptorSetLayoutCreateInfo, nullptr, &m_pDescriptorSetLayout) != VK_SUCCESS)
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

	if (vkCreatePipelineLayout(m_Device, &PipelineLayoutCreateInfo, nullptr, &m_pPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createGraphicsPipeline()
{
	hiveVKT::CVkShaderModuleCreator ShaderModuleCreator;
	auto VertexShaderModule = ShaderModuleCreator.createUnique(m_Device, "vert.spv");
	auto FragmentShaderModule = ShaderModuleCreator.createUnique(m_Device, "frag.spv");

	hiveVKT::CVkGraphicsPipelineCreator PipelineCreator(m_SwapChainExtent.width, m_SwapChainExtent.height);

	PipelineCreator.addShaderStage(vk::ShaderStageFlagBits::eVertex, VertexShaderModule.get());
	PipelineCreator.addShaderStage(vk::ShaderStageFlagBits::eFragment, FragmentShaderModule.get());

	PipelineCreator.addVertexBinding({ 0, sizeof(SVertex) , vk::VertexInputRate::eVertex });
	PipelineCreator.addVertexAttribute({ 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(SVertex, SVertex::Position) });
	PipelineCreator.addVertexAttribute({ 1, 0, vk::Format::eR32G32B32Sfloat , offsetof(SVertex, SVertex::Color) });
	PipelineCreator.addVertexAttribute({ 2, 0, vk::Format::eR32G32Sfloat , offsetof(SVertex, SVertex::TexCoord) });

	PipelineCreator.setFrontFace(vk::FrontFace::eCounterClockwise);

	PipelineCreator.setMultisampleState({ {}, vk::SampleCountFlagBits(m_SampleCount), VK_TRUE, .2f, nullptr, VK_FALSE, VK_FALSE });

	PipelineCreator.setDepthTestEnable(VK_TRUE);
	PipelineCreator.setDepthCompareOp(vk::CompareOp::eLess);

	m_pGraphicsPipeline = PipelineCreator.create(m_Device, m_pPipelineLayout, nullptr, m_pRenderPass);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createCommandPool()
{
	SQueueFamilyIndices QueueFamilyIndices = __findRequiredQueueFamilies(m_pPhysicalDevice);

	VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndices.QueueFamily.value();

	if (vkCreateCommandPool(m_Device, &CommandPoolCreateInfo, nullptr, &m_pCommandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createMsaaResource()
{
	__createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1, m_SampleCount, m_SwapChainImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pMsaaImage, m_pMsaaImageDeviceMemory);

	m_pMsaaImageView = __createImageView(m_pMsaaImage, m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	__transitionImageLayout(m_pMsaaImage, m_SwapChainImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createDepthResources()
{
	VkFormat DepthImageFormat = __findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	__createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1, m_SampleCount, DepthImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pDepthImage, m_pDepthImageDeviceMemory);
	m_pDepthImageView = __createImageView(m_pDepthImage, DepthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	__transitionImageLayout(m_pDepthImage, DepthImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createImage(uint32_t vImageWidth, uint32_t vImageHeight, uint32_t vMipmapLevel, VkSampleCountFlagBits vSampleCount, VkFormat vImageFormat, VkImageTiling vImageTiling, VkImageUsageFlags vImageUsages, VkMemoryPropertyFlags vMemoryProperties, VkImage& vImage, VkDeviceMemory& vImageDeviceMemory)
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

	if (vkCreateImage(m_Device, &ImageCreateInfo, nullptr, &vImage) != VK_SUCCESS)
		throw std::runtime_error("Failed to create image!");

	VkMemoryRequirements MemoryRequirements = {};
	vkGetImageMemoryRequirements(m_Device, vImage, &MemoryRequirements);

	VkMemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = __findMemoryType(MemoryRequirements.memoryTypeBits, vMemoryProperties);

	if (vkAllocateMemory(m_Device, &MemoryAllocateInfo, nullptr, &vImageDeviceMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate memory for image!");

	vkBindImageMemory(m_Device, vImage, vImageDeviceMemory, 0);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__transitionImageLayout(VkImage vImage, VkFormat vImageFormat, VkImageLayout vOldImageLayout, VkImageLayout vNewImageLayout, uint32_t vMipmapLevel)
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
void VulkanApp::CPerpixelShadingApp::__createFramebuffers()
{
	m_FramebufferSet.resize(m_SwapChainImageSet.size());

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		std::array<VkImageView, 3> Attachments = { m_pMsaaImageView,m_pDepthImageView,m_SwapChainImageViewSet[i] };

		VkFramebufferCreateInfo FramebufferCreateInfo = {};
		FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferCreateInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
		FramebufferCreateInfo.pAttachments = Attachments.data();
		FramebufferCreateInfo.renderPass = m_pRenderPass;
		FramebufferCreateInfo.layers = 1;
		FramebufferCreateInfo.width = m_SwapChainExtent.width;
		FramebufferCreateInfo.height = m_SwapChainExtent.height;

		if (vkCreateFramebuffer(m_Device, &FramebufferCreateInfo, nullptr, &m_FramebufferSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create frame buffer!");
	}
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__createTextureSamplerResources()
{
	int TextureWidth = 0, TextureHeight = 0, TextureChannels = 0;
	unsigned char* Pixels = stbi_load("../../resource/models/chalet/Chalet.jpg", &TextureWidth, &TextureHeight, &TextureChannels, STBI_rgb_alpha);
	if (!Pixels)
		throw std::runtime_error("Failed to load texture image!");

	m_MipmapLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(TextureWidth, TextureHeight)))) + 1;

	VkDeviceSize ImageSize = TextureWidth * TextureHeight * 4;

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);
	void* Data = nullptr;
	vkMapMemory(m_Device, pStagingBufferDeviceMemory, 0, ImageSize, 0, &Data);
	memcpy(Data, Pixels, static_cast<size_t>(ImageSize));
	vkUnmapMemory(m_Device, pStagingBufferDeviceMemory);

	stbi_image_free(Pixels);

	__createImage(static_cast<uint32_t>(TextureWidth), static_cast<uint32_t>(TextureHeight), m_MipmapLevel, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pTextureImage, m_pTextureImageDeviceMemory);
	__transitionImageLayout(m_pTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipmapLevel);
	__copyBuffer2Image(pStagingBuffer, m_pTextureImage, static_cast<uint32_t>(TextureWidth), static_cast<uint32_t>(TextureHeight));
	__generateMipmaps(m_pTextureImage, VK_FORMAT_R8G8B8A8_UNORM, TextureWidth, TextureHeight, m_MipmapLevel);

	vkDestroyBuffer(m_Device, pStagingBuffer, nullptr);
	vkFreeMemory(m_Device, pStagingBufferDeviceMemory, nullptr);

	m_pTextureImageView = __createImageView(m_pTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_MipmapLevel);

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

	if (vkCreateSampler(m_Device, &SamplerCreateInfo, nullptr, &m_pTextureSampler) != VK_SUCCESS)
		throw std::runtime_error("Failed to create texture sampler!");
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__generateMipmaps(VkImage vImage, VkFormat vImageFormat, int32_t vImageWidth, int32_t vImageHeight, uint32_t vMipmapLevel)
{
	VkFormatProperties FormatProperties;
	vkGetPhysicalDeviceFormatProperties(m_pPhysicalDevice, vImageFormat, &FormatProperties);
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
void VulkanApp::CPerpixelShadingApp::__createBuffer(VkDeviceSize vBufferSize, VkBufferUsageFlags vBufferUsage, VkMemoryPropertyFlags vMemoryProperty, VkBuffer& voBuffer, VkDeviceMemory& voBufferDeviceMemory)
{
	VkBufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.size = vBufferSize;
	BufferCreateInfo.usage = vBufferUsage;
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_Device, &BufferCreateInfo, nullptr, &voBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create vertex buffer!");

	VkMemoryRequirements MemoryRequirements = {};
	vkGetBufferMemoryRequirements(m_Device, voBuffer, &MemoryRequirements);

	VkMemoryAllocateInfo MemoryAllocateInfo = {};
	MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocateInfo.memoryTypeIndex = __findMemoryType(MemoryRequirements.memoryTypeBits, vMemoryProperty);

	if (vkAllocateMemory(m_Device, &MemoryAllocateInfo, nullptr, &voBufferDeviceMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate memory for vertex buffer!");

	vkBindBufferMemory(m_Device, voBuffer, voBufferDeviceMemory, 0);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__copyBuffer2Image(VkBuffer vBuffer, VkImage vImage, uint32_t vImageWidth, uint32_t vImageHeight)
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
void VulkanApp::CPerpixelShadingApp::__createVertexBuffer()
{
	VkDeviceSize BufferSize = sizeof(m_VertexData[0]) * m_VertexData.size();

	VkBuffer pStagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory pStagingBufferDeviceMemory = VK_NULL_HANDLE;

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(m_Device, pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, m_VertexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(m_Device, pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pVertexBuffer, m_pVertexBufferDeviceMemory);

	__copyBuffer(pStagingBuffer, m_pVertexBuffer, BufferSize);

	vkDestroyBuffer(m_Device, pStagingBuffer, nullptr);
	vkFreeMemory(m_Device, pStagingBufferDeviceMemory, nullptr);
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
	vkMapMemory(m_Device, pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, m_IndexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(m_Device, pStagingBufferDeviceMemory);

	__createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_pIndexBuffer, m_pIndexBufferMemory);

	__copyBuffer(pStagingBuffer, m_pIndexBuffer, BufferSize);

	vkDestroyBuffer(m_Device, pStagingBuffer, nullptr);
	vkFreeMemory(m_Device, pStagingBufferDeviceMemory, nullptr);
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

	if (vkCreateDescriptorPool(m_Device, &DescriptorPoolCreateInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
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
	if (vkAllocateDescriptorSets(m_Device, &DescriptorSetAllocateInfo, m_DescriptorSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor set!");

	for (auto i = 0; i < m_SwapChainImageSet.size(); ++i)
	{
		VkDescriptorBufferInfo DescriptorBufferInfo = {};
		DescriptorBufferInfo.buffer = m_UniformBufferSet[i];
		DescriptorBufferInfo.offset = 0;
		DescriptorBufferInfo.range = sizeof(SUniformBufferObject);

		VkDescriptorImageInfo DescriptorImageInfo = {};
		DescriptorImageInfo.imageView = m_pTextureImageView;
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

		vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(WriteDescriptors.size()), WriteDescriptors.data(), 0, nullptr);
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

	if (vkAllocateCommandBuffers(m_Device, &CommandBufferAllocateInfo, m_CommandBufferSet.data()) != VK_SUCCESS)
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
		RenderPassBeginInfo.renderArea.extent = m_SwapChainExtent;

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
		if (vkCreateSemaphore(m_Device, &SemaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphoreSet[i]) != VK_SUCCESS || vkCreateSemaphore(m_Device, &SemaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphoreSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphores!");

		if (vkCreateFence(m_Device, &FenceCreateInfo, nullptr, &m_InFlightFenceSet[i]) != VK_SUCCESS)
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

	SUniformBufferObject UBO = {};
	UBO.Model = glm::rotate(glm::mat4(1.0f), Time*glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	UBO.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	UBO.Projection = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / static_cast<float>(m_SwapChainExtent.height), 0.1f, 10.0f);

	UBO.Projection[1][1] *= -1;

	void* Data = nullptr;
	vkMapMemory(m_Device, m_UniformBufferDeviceMemorySet[vImageIndex], 0, sizeof(UBO), 0, &Data);
	memcpy(Data, &UBO, sizeof(UBO));
	vkUnmapMemory(m_Device, m_UniformBufferDeviceMemorySet[vImageIndex]);
}

//************************************************************************************
//Function:
void VulkanApp::CPerpixelShadingApp::__loadModel()
{
	tinyobj::attrib_t Attribute;
	std::vector<tinyobj::shape_t> ShapeSet;
	std::vector<tinyobj::material_t> MaterialSet;
	std::string Warn, Error;

	if (!tinyobj::LoadObj(&Attribute, &ShapeSet, &MaterialSet, &Warn, &Error, "../../resource/models/chalet/Chalet.obj"))
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
bool VulkanApp::CPerpixelShadingApp::__checkPhysicalDeviceExtensionSupport(const VkPhysicalDevice& vPhysicalDevice)const
{
	uint32_t PhysicalDeviceExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &PhysicalDeviceExtensionCount, nullptr);
	std::vector<VkExtensionProperties> PhysicalDeviceExtensionPropertySet(PhysicalDeviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &PhysicalDeviceExtensionCount, PhysicalDeviceExtensionPropertySet.data());

	std::set<std::string> RequiredPhysicalDeviceExtensionSet(m_enabledExtensionsAtDeviceLevel.begin(), m_enabledExtensionsAtDeviceLevel.end());

	for (const auto& ExtensionProperty : PhysicalDeviceExtensionPropertySet)
		RequiredPhysicalDeviceExtensionSet.erase(ExtensionProperty.extensionName);

	return RequiredPhysicalDeviceExtensionSet.empty();
}

//************************************************************************************
//Function:
bool VulkanApp::CPerpixelShadingApp::__isPhysicalDeviceSuitable(const VkPhysicalDevice& vPhysicalDevice)const
{
	SQueueFamilyIndices QueueFamily = __findRequiredQueueFamilies(vPhysicalDevice);
	bool IsExtensionSupport = __checkPhysicalDeviceExtensionSupport(vPhysicalDevice);
	bool IsSwapChainAdequate = false;
	if (IsExtensionSupport)
	{
		SSwapChainSupportDetails SwapChainSupportDetails = __queryPhysicalDeviceSwapChainSupport(vPhysicalDevice);
		IsSwapChainAdequate = !SwapChainSupportDetails.SurfaceFormatSet.empty() && !SwapChainSupportDetails.PresentModeSet.empty();
	}

	return QueueFamily.IsComplete() && IsExtensionSupport && IsSwapChainAdequate;
}

//************************************************************************************
//Function:
VulkanApp::SQueueFamilyIndices VulkanApp::CPerpixelShadingApp::__findRequiredQueueFamilies(const VkPhysicalDevice& vPhysicalDevice)const
{
	SQueueFamilyIndices RequiredQueueFamilyIndices;

	uint32_t QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &QueueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> QueueFamilyPropertySet(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &QueueFamilyCount, QueueFamilyPropertySet.data());

	int i = 0;
	for (const auto& QueueFamilyProperty : QueueFamilyPropertySet)
	{
		VkBool32 GraphicsSupport = VK_FALSE, PresentSupport = VK_FALSE, TransferSupport = VK_FALSE;

		GraphicsSupport = static_cast<VkBool32>(QueueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT);
		vkGetPhysicalDeviceSurfaceSupportKHR(vPhysicalDevice, i, m_pSurface, &PresentSupport);
		TransferSupport = static_cast<VkBool32>(QueueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT);

		if (GraphicsSupport && PresentSupport && TransferSupport)
		{
			RequiredQueueFamilyIndices.QueueFamily = i;
			return RequiredQueueFamilyIndices;
		}
	}

	return RequiredQueueFamilyIndices;
}

//************************************************************************************
//Function:
VulkanApp::SSwapChainSupportDetails VulkanApp::CPerpixelShadingApp::__queryPhysicalDeviceSwapChainSupport(const VkPhysicalDevice& vPhysicalDevice)const
{
	SSwapChainSupportDetails SwapChainSuportDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, m_pSurface, &SwapChainSuportDetails.SurfaceCapabilities);

	uint32_t FormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, m_pSurface, &FormatCount, nullptr);
	SwapChainSuportDetails.SurfaceFormatSet.resize(FormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, m_pSurface, &FormatCount, SwapChainSuportDetails.SurfaceFormatSet.data());

	uint32_t PresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, m_pSurface, &PresentModeCount, nullptr);
	SwapChainSuportDetails.PresentModeSet.resize(PresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, m_pSurface, &PresentModeCount, SwapChainSuportDetails.PresentModeSet.data());

	return SwapChainSuportDetails;
}

//************************************************************************************
//Function:
VkSurfaceFormatKHR VulkanApp::CPerpixelShadingApp::__determineSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& vCandidateSurfaceFormatSet)const
{
	if (vCandidateSurfaceFormatSet.size() == 1 && vCandidateSurfaceFormatSet[0].format == VK_FORMAT_UNDEFINED)
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	for (const auto& Format : vCandidateSurfaceFormatSet)
	{
		if (Format.format == VK_FORMAT_B8G8R8A8_UNORM && Format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return Format;
	}

	return vCandidateSurfaceFormatSet[0];
}

//************************************************************************************
//Function:
VkPresentModeKHR VulkanApp::CPerpixelShadingApp::__determinePresentMode(const std::vector<VkPresentModeKHR>& vCandidatePresentModeSet)const
{
	VkPresentModeKHR BestPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& PresentMode : vCandidatePresentModeSet)
	{
		if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return PresentMode;
		else if (PresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			BestPresentMode = PresentMode;
	}

	return BestPresentMode;
}

//************************************************************************************
//Function:
VkExtent2D VulkanApp::CPerpixelShadingApp::__determineSwapChainExtent(const VkSurfaceCapabilitiesKHR& vSurfaceCapabilities)const
{
	if (vSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return vSurfaceCapabilities.currentExtent;
	}
	else
	{
		int WindowWidth = 0, WindowHeight = 0;
		glfwGetFramebufferSize(m_pGLFWWindow, &WindowWidth, &WindowHeight);

		VkExtent2D ActualExtent = { static_cast<uint32_t>(WindowWidth), static_cast<uint32_t>(WindowHeight) };

		ActualExtent.width = std::max(vSurfaceCapabilities.minImageExtent.width, std::min(vSurfaceCapabilities.maxImageExtent.width, ActualExtent.width));
		ActualExtent.height = std::max(vSurfaceCapabilities.minImageExtent.height, std::min(vSurfaceCapabilities.maxImageExtent.height, ActualExtent.height));

		return ActualExtent;
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
	if (vkCreateImageView(m_Device, &ImageViewCreateInfo, nullptr, &ImageView) != VK_SUCCESS)
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
		vkGetPhysicalDeviceFormatProperties(m_pPhysicalDevice, Format, &FormatProperties);

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
	vkGetPhysicalDeviceMemoryProperties(m_pPhysicalDevice, &PhysicalDeviceMemoryProperties);

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
	vkGetPhysicalDeviceProperties(m_pPhysicalDevice, &PhysicalDeviceProperties);

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
	vkAllocateCommandBuffers(m_Device, &CommandBufferAllocateInfo, &CommandBuffer);

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

	vkFreeCommandBuffers(m_Device, m_pCommandPool, 1, &vCommandBuffer);
}

//************************************************************************************
//Function:
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanApp::CPerpixelShadingApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, VkDebugUtilsMessageTypeFlagsEXT vMessageTypeFlags, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (vMessageSeverityFlags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		std::cout << "Validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}
