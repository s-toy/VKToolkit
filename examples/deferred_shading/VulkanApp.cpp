#include "VulkanApp.h"
#include <set>
#include <iostream>
#include <chrono>
#include <unordered_map>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/gtc/matrix_transform.hpp>
#include "VkRenderPassCreator.hpp"
#include "VkGraphicsPipelineCreator.hpp"
#include "VkShaderModuleCreator.hpp"
#include "Camera.hpp"
#include "VkUtility.hpp"
#include "Model.hpp"

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::_awakeV()
{
	fetchEnabledPhysicalDeviceFeatures().samplerAnisotropy = VK_TRUE;
	fetchEnabledPhysicalDeviceFeatures().sampleRateShading = VK_TRUE;
}

//************************************************************************************
//Function:
bool DeferredShading::CDeferredShadingApp::_initV()
{
	__loadModel();
	__createVertexBuffers();
	__createIndexBuffers();

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
void DeferredShading::CDeferredShadingApp::_updateV()
{
	vkWaitForFences(m_VkContext.getDevice(), 1, &m_InFlightFenceSet[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t ImageIndex = 0;
	VkResult Result = vkAcquireNextImageKHR(m_VkContext.getDevice(), m_VkContext.getSwapchainKHR(), std::numeric_limits<uint64_t>::max(), m_ImageAvailableSemaphoreSet[m_CurrentFrame], VK_NULL_HANDLE, &ImageIndex);
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
	if (vkQueueSubmit(m_VkContext.getQueue(), 1, &SubmitInfo, nullptr) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer!");

	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = SignalSemaphores_OffScreen;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &m_DeferredCommandBufferSet[ImageIndex];
	VkSemaphore SignalSemaphores_Deferred[] = { m_RenderFinishedSemaphoreSet[m_CurrentFrame] };
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = SignalSemaphores_Deferred;
	vkResetFences(m_VkContext.getDevice(), 1, &m_InFlightFenceSet[m_CurrentFrame]);
	if (vkQueueSubmit(m_VkContext.getQueue(), 1, &SubmitInfo, m_InFlightFenceSet[m_CurrentFrame]) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer!");

	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores_Deferred;
	VkSwapchainKHR SwapChains[] = { m_VkContext.getSwapchainKHR() };
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = SwapChains;
	PresentInfo.pImageIndices = &ImageIndex;
	PresentInfo.pResults = nullptr;
	Result = vkQueuePresentKHR(m_VkContext.getQueue(), &PresentInfo);
	if (Result != VK_SUCCESS)
		throw std::runtime_error("Failed to present swap chain image!");

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::_destroyV()
{
	m_VkContext.getDevice().waitIdle();
	__cleanup();
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__cleanup()
{
	for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_VkContext.getDevice(), m_OffScreenRenderingFinishedSemaphoreSet[i], nullptr);
		vkDestroySemaphore(m_VkContext.getDevice(), m_ImageAvailableSemaphoreSet[i], nullptr);
		vkDestroySemaphore(m_VkContext.getDevice(), m_RenderFinishedSemaphoreSet[i], nullptr);
		vkDestroyFence(m_VkContext.getDevice(), m_InFlightFenceSet[i], nullptr);
	}

	vkDestroyDescriptorPool(m_VkContext.getDevice(), m_pDescriptorPool, nullptr);

	for (auto i = 0; i < m_VkContext.getSwapChainImageSize(); ++i)
	{
		vkDestroyBuffer(m_VkContext.getDevice(), m_UniformBufferSet_OffScreen[i], nullptr);
		vkFreeMemory(m_VkContext.getDevice(), m_UniformBufferDeviceMemorySet_OffScreen[i], nullptr);
		vkDestroyBuffer(m_VkContext.getDevice(), m_UniformBufferSet_Deferred[i], nullptr);
		vkFreeMemory(m_VkContext.getDevice(), m_UniformBufferDeviceMemorySet_Deferred[i], nullptr);

		vkDestroyFramebuffer(m_VkContext.getDevice(), m_DeferredFramebufferSet[i], nullptr);
	}

	vkDestroySampler(m_VkContext.getDevice(), m_pSampler4DeferredRendering, nullptr);

	m_pModel->destroy(m_VkContext.getDevice());
	delete m_pModel;

	vkDestroyBuffer(m_VkContext.getDevice(), m_pVertexBuffer_Quad, nullptr);
	vkFreeMemory(m_VkContext.getDevice(), m_pVertexBufferDeviceMemory_Quad, nullptr);
	vkDestroyBuffer(m_VkContext.getDevice(), m_pIndexBuffer_Quad, nullptr);
	vkFreeMemory(m_VkContext.getDevice(), m_pIndexBufferMemory_Quad, nullptr);

	m_PositionAttachment.destroy(m_VkContext.getDevice());
	m_NormalAttachment.destroy(m_VkContext.getDevice());
	m_ColorAttachment.destroy(m_VkContext.getDevice());
	m_DepthAttachment.destroy(m_VkContext.getDevice());
	vkDestroyFramebuffer(m_VkContext.getDevice(), m_pOffScreenFramebuffer, nullptr);

	vkDestroyPipeline(m_VkContext.getDevice(), m_pDeferredPipeline, nullptr);
	vkDestroyPipelineLayout(m_VkContext.getDevice(), m_pDeferredPipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(m_VkContext.getDevice(), m_pDeferredDescriptorSetLayout, nullptr);
	vkDestroyRenderPass(m_VkContext.getDevice(), m_pDeferredRenderPass, nullptr);

	vkDestroyPipeline(m_VkContext.getDevice(), m_pOffScreenPipeline, nullptr);
	vkDestroyPipelineLayout(m_VkContext.getDevice(), m_pOffScreenPipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(m_VkContext.getDevice(), m_pOffScreenDescriptorSetLayout, nullptr);
	vkDestroyRenderPass(m_VkContext.getDevice(), m_pOffScreenRenderPass, nullptr);
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
	SubpassDependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	SubpassDependency.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
	SubpassDependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

	RenderPassCreator.addSubpassDependency(SubpassDependency);

	m_pOffScreenRenderPass = RenderPassCreator.create(m_VkContext.getDevice());
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding UniformBufferBinding = {};
	UniformBufferBinding.binding = 0;
	UniformBufferBinding.descriptorCount = 1;
	UniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	UniformBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	UniformBufferBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 1> Bindings = { UniformBufferBinding };

	VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
	DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
	DescriptorSetLayoutCreateInfo.pBindings = Bindings.data();

	if (vkCreateDescriptorSetLayout(m_VkContext.getDevice(), &DescriptorSetLayoutCreateInfo, nullptr, &m_pOffScreenDescriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor set layout!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenPipelineLayout()
{
	auto ModelDescriptorSetLayout = m_pModel->getModelDescriptorSetLayout();

	std::array<VkDescriptorSetLayout, 2> DescriptorSetLayouts = { m_pOffScreenDescriptorSetLayout,ModelDescriptorSetLayout };

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {};
	PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutCreateInfo.setLayoutCount = 2;
	PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts.data();
	PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_VkContext.getDevice(), &PipelineLayoutCreateInfo, nullptr, &m_pOffScreenPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredRenderPass()
{
	hiveVKT::CVkRenderPassCreator RenderPassCreator;
	RenderPassCreator.addAttachment(m_VkContext.getSwapchainImageFormat(), vk::ImageLayout::ePresentSrcKHR);
	const std::vector<vk::AttachmentReference> ColorAttachmentReferences = { {0, vk::ImageLayout::eColorAttachmentOptimal} };
	RenderPassCreator.addSubpass(ColorAttachmentReferences, {}, {}, {}, {});

	vk::SubpassDependency SubpassDependency;
	SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	SubpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	SubpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	SubpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	RenderPassCreator.addSubpassDependency(SubpassDependency);

	m_pDeferredRenderPass = RenderPassCreator.create(m_VkContext.getDevice());
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

	if (vkCreateDescriptorSetLayout(m_VkContext.getDevice(), &DescriptorSetLayoutCreateInfo, nullptr, &m_pDeferredDescriptorSetLayout) != VK_SUCCESS)
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

	if (vkCreatePipelineLayout(m_VkContext.getDevice(), &PipelineLayoutCreateInfo, nullptr, &m_pDeferredPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createGraphicsPipelines()
{
	hiveVKT::CVkGraphicsPipelineCreator PipelineCreator;

	//off-screen
	hiveVKT::CVkShaderModuleCreator ShaderModuleCreator;
	auto OffScreenVertexShaderModule = ShaderModuleCreator.createUnique(m_VkContext.getDevice(), "OffScreen_vert.spv");
	auto OffScreenFragmentShaderModule = ShaderModuleCreator.createUnique(m_VkContext.getDevice(), "OffScreen_frag.spv");
	PipelineCreator.addShaderStage(vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, OffScreenVertexShaderModule.get(), "main"));
	PipelineCreator.addShaderStage(vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, OffScreenFragmentShaderModule.get(), "main"));

	PipelineCreator.addViewport(vk::Viewport(0, 0, m_VkContext.getSwapchainExtent().width, m_VkContext.getSwapchainExtent().height, 0, 1.0));
	PipelineCreator.addScissor(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(m_VkContext.getSwapchainExtent().width, m_VkContext.getSwapchainExtent().height)));

	auto BindingDescription = SVertex::getBindingDescription();
	auto AttributeDescription = SVertex::getAttributeDescription();
	PipelineCreator.addVertexBinding(BindingDescription);
	for (auto Attribute : AttributeDescription)
		PipelineCreator.addVertexAttribute(Attribute);

	PipelineCreator.fetchRasterizationState().setCullMode(vk::CullModeFlagBits::eBack);

	PipelineCreator.fetchMultisampleState().setSampleShadingEnable(true);
	PipelineCreator.fetchMultisampleState().setRasterizationSamples(vk::SampleCountFlagBits::e1);
	PipelineCreator.fetchMultisampleState().setMinSampleShading(.2f);

	PipelineCreator.fetchDepthStencilState().setDepthTestEnable(true);
	PipelineCreator.fetchDepthStencilState().setDepthWriteEnable(true);
	PipelineCreator.fetchDepthStencilState().setDepthCompareOp(vk::CompareOp::eLessOrEqual);

	for (auto i = 0; i < 3; ++i)
		PipelineCreator.addColorBlendAttachment(DefaultPipelineColorBlendAttachmentState);

	m_pOffScreenPipeline = PipelineCreator.create(m_VkContext.getDevice(), m_pOffScreenPipelineLayout, nullptr, m_pOffScreenRenderPass, 0);

	//deferred
	PipelineCreator.clearShaderStage();
	auto DeferredVertexShaderModule = ShaderModuleCreator.createUnique(m_VkContext.getDevice(), "Deferred_vert.spv");
	auto DeferredFragmentShaderModule = ShaderModuleCreator.createUnique(m_VkContext.getDevice(), "Deferred_frag.spv");
	PipelineCreator.addShaderStage(vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, DeferredVertexShaderModule.get(), "main"));
	PipelineCreator.addShaderStage(vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, DeferredFragmentShaderModule.get(), "main"));

	PipelineCreator.clearVertexInputInfo();
	auto BindingDescription_Deferred = SQuadVertex::getBindingDescription();
	auto AttributeDescription_Deferred = SQuadVertex::getAttributeDescription();
	PipelineCreator.addVertexBinding(BindingDescription_Deferred);
	for (auto Attribute : AttributeDescription_Deferred)
		PipelineCreator.addVertexAttribute(Attribute);

	PipelineCreator.fetchDepthStencilState().setDepthTestEnable(false);

	PipelineCreator.clearColorBlendAttachment();
	PipelineCreator.addColorBlendAttachment(DefaultPipelineColorBlendAttachmentState);

	m_pDeferredPipeline = PipelineCreator.create(m_VkContext.getDevice(), m_pDeferredPipelineLayout, nullptr, m_pDeferredRenderPass, 0);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenRenderTargets()
{
	vk::ImageCreateInfo ImageCreateInfo = {
		vk::ImageCreateFlags(),
		vk::ImageType::e2D,
		vk::Format::eR16G16B16A16Sfloat,
		vk::Extent3D(m_VkContext.getSwapchainExtent().width, m_VkContext.getSwapchainExtent().height,1),
		1,1,vk::SampleCountFlagBits::e1,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled
	};

	m_PositionAttachment.create(m_VkContext.getDevice(), ImageCreateInfo, vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor, false);
	hiveVKT::executeImmediately(m_VkContext.getDevice(), m_VkContext.getCommandPool(), m_VkContext.getQueue(), [&](vk::CommandBuffer vCommandBuffer) {
		vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eColor,0,1,0,1 };
		m_PositionAttachment.translateImageLayout(vCommandBuffer, vk::ImageLayout::eColorAttachmentOptimal, TranslateRange);
	});

	m_NormalAttachment.create(m_VkContext.getDevice(), ImageCreateInfo, vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor, false);
	hiveVKT::executeImmediately(m_VkContext.getDevice(), m_VkContext.getCommandPool(), m_VkContext.getQueue(), [&](vk::CommandBuffer vCommandBuffer) {
		vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eColor,0,1,0,1 };
		m_NormalAttachment.translateImageLayout(vCommandBuffer, vk::ImageLayout::eColorAttachmentOptimal, TranslateRange);
	});

	ImageCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
	m_ColorAttachment.create(m_VkContext.getDevice(), ImageCreateInfo, vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor, false);
	hiveVKT::executeImmediately(m_VkContext.getDevice(), m_VkContext.getCommandPool(), m_VkContext.getQueue(), [&](vk::CommandBuffer vCommandBuffer) {
		vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eColor,0,1,0,1 };
		m_ColorAttachment.translateImageLayout(vCommandBuffer, vk::ImageLayout::eColorAttachmentOptimal, TranslateRange);
	});

	ImageCreateInfo.format = (vk::Format)__findSupportedFormat(gDepthFormatSet, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	ImageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	m_DepthAttachment.create(m_VkContext.getDevice(), ImageCreateInfo, vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eDepth, false);
	hiveVKT::executeImmediately(m_VkContext.getDevice(), m_VkContext.getCommandPool(), m_VkContext.getQueue(), [&](vk::CommandBuffer vCommandBuffer) {
		vk::ImageSubresourceRange TranslateRange = { vk::ImageAspectFlagBits::eDepth,0,1,0,1 };
		m_ColorAttachment.translateImageLayout(vCommandBuffer, vk::ImageLayout::eDepthStencilAttachmentOptimal, TranslateRange);
	});
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenFramebuffer()
{
	std::array<VkImageView, 4> Attachments = { m_PositionAttachment.getImageView(),m_NormalAttachment.getImageView(),m_ColorAttachment.getImageView(),m_DepthAttachment.getImageView() };

	VkFramebufferCreateInfo FramebufferCreateInfo = {};
	FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FramebufferCreateInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
	FramebufferCreateInfo.pAttachments = Attachments.data();
	FramebufferCreateInfo.renderPass = m_pOffScreenRenderPass;
	FramebufferCreateInfo.layers = 1;
	FramebufferCreateInfo.width = m_VkContext.getSwapchainExtent().width;
	FramebufferCreateInfo.height = m_VkContext.getSwapchainExtent().height;

	if (vkCreateFramebuffer(m_VkContext.getDevice(), &FramebufferCreateInfo, nullptr, &m_pOffScreenFramebuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create frame buffer!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredFramebuffers()
{
	m_DeferredFramebufferSet.resize(m_VkContext.getSwapChainImageSize());

	for (auto i = 0; i < m_VkContext.getSwapChainImageSize(); ++i)
	{
		vk::FramebufferCreateInfo FramebufferCreateInfo = {
			vk::FramebufferCreateFlags(),
			m_pDeferredRenderPass,1,
			&m_VkContext.getSwapChainImageViewAt(i),
			m_VkContext.getSwapchainExtent().width,m_VkContext.getSwapchainExtent().height,1
		};

		m_DeferredFramebufferSet[i] = m_VkContext.getDevice().createFramebuffer(FramebufferCreateInfo);
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createVertexBuffers()
{
	vk::Buffer pStagingBuffer;
	vk::DeviceMemory pStagingBufferDeviceMemory;

	vk::DeviceSize BufferSize = sizeof(gQuadVertexData[0]) * gQuadVertexData.size();

	hiveVKT::createBuffer(m_VkContext.getDevice(), BufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(m_VkContext.getDevice(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, gQuadVertexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(m_VkContext.getDevice(), pStagingBufferDeviceMemory);

	hiveVKT::createBuffer(m_VkContext.getDevice(), BufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, m_pVertexBuffer_Quad, m_pVertexBufferDeviceMemory_Quad);

	__copyBuffer(pStagingBuffer, m_pVertexBuffer_Quad, BufferSize);

	vkDestroyBuffer(m_VkContext.getDevice(), pStagingBuffer, nullptr);
	vkFreeMemory(m_VkContext.getDevice(), pStagingBufferDeviceMemory, nullptr);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createIndexBuffers()
{
	vk::Buffer pStagingBuffer = nullptr;
	vk::DeviceMemory pStagingBufferDeviceMemory = nullptr;

	vk::DeviceSize BufferSize = sizeof(gQuadIndexData[0]) * gQuadIndexData.size();

	hiveVKT::createBuffer(m_VkContext.getDevice(), BufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, pStagingBuffer, pStagingBufferDeviceMemory);

	void* Data = nullptr;
	vkMapMemory(m_VkContext.getDevice(), pStagingBufferDeviceMemory, 0, BufferSize, 0, &Data);
	memcpy(Data, gQuadIndexData.data(), static_cast<size_t>(BufferSize));
	vkUnmapMemory(m_VkContext.getDevice(), pStagingBufferDeviceMemory);

	hiveVKT::createBuffer(m_VkContext.getDevice(), BufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, m_pIndexBuffer_Quad, m_pIndexBufferMemory_Quad);

	__copyBuffer(pStagingBuffer, m_pIndexBuffer_Quad, BufferSize);

	vkDestroyBuffer(m_VkContext.getDevice(), pStagingBuffer, nullptr);
	vkFreeMemory(m_VkContext.getDevice(), pStagingBufferDeviceMemory, nullptr);
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

	if (vkCreateSampler(m_VkContext.getDevice(), &SamplerCreateInfo, nullptr, &m_pSampler4DeferredRendering) != VK_SUCCESS)
		throw std::runtime_error("Failed to create texture sampler!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createUniformBuffers()
{
	VkDeviceSize BufferSize_OffScreen = sizeof(SUniformBufferObject_OffScreen);
	VkDeviceSize BufferSize_Deferred = sizeof(SUniformBufferObject_Deferred);

	m_UniformBufferSet_OffScreen.resize(m_VkContext.getSwapChainImageSize());
	m_UniformBufferDeviceMemorySet_OffScreen.resize(m_VkContext.getSwapChainImageSize());
	m_UniformBufferSet_Deferred.resize(m_VkContext.getSwapChainImageSize());
	m_UniformBufferDeviceMemorySet_Deferred.resize(m_VkContext.getSwapChainImageSize());

	for (auto i = 0; i < m_VkContext.getSwapChainImageSize(); ++i)
	{
		hiveVKT::createBuffer(m_VkContext.getDevice(), BufferSize_OffScreen, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, m_UniformBufferSet_OffScreen[i], m_UniformBufferDeviceMemorySet_OffScreen[i]);
		hiveVKT::createBuffer(m_VkContext.getDevice(), BufferSize_Deferred, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, m_UniformBufferSet_Deferred[i], m_UniformBufferDeviceMemorySet_Deferred[i]);
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDescriptorPool()
{
	uint32_t NumImageInSwapChain = static_cast<uint32_t>(m_VkContext.getSwapChainImageSize());
	uint32_t NumUBO = NumImageInSwapChain * (1 + 1);
	uint32_t NumSampler = NumImageInSwapChain * 3;
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

	if (vkCreateDescriptorPool(m_VkContext.getDevice(), &DescriptorPoolCreateInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor pool!");
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> DescriptorSetLayoutSet(m_VkContext.getSwapChainImageSize(), m_pOffScreenDescriptorSetLayout);

	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
	DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorSetAllocateInfo.descriptorPool = m_pDescriptorPool;
	DescriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(m_VkContext.getSwapChainImageSize());
	DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayoutSet.data();

	m_OffScreenDescriptorSet.resize(m_VkContext.getSwapChainImageSize());
	if (vkAllocateDescriptorSets(m_VkContext.getDevice(), &DescriptorSetAllocateInfo, m_OffScreenDescriptorSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor set!");

	for (auto i = 0; i < m_VkContext.getSwapChainImageSize(); ++i)
	{
		//model, view and projection 
		VkDescriptorBufferInfo DescriptorBufferInfo = {};
		DescriptorBufferInfo.buffer = m_UniformBufferSet_OffScreen[i];
		DescriptorBufferInfo.offset = 0;
		DescriptorBufferInfo.range = sizeof(SUniformBufferObject_OffScreen);

		std::array<VkWriteDescriptorSet, 1> WriteDescriptors = {};
		WriteDescriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		WriteDescriptors[0].dstSet = m_OffScreenDescriptorSet[i];
		WriteDescriptors[0].dstBinding = 0;
		WriteDescriptors[0].dstArrayElement = 0;
		WriteDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		WriteDescriptors[0].descriptorCount = 1;
		WriteDescriptors[0].pBufferInfo = &DescriptorBufferInfo;
		WriteDescriptors[0].pImageInfo = nullptr;
		WriteDescriptors[0].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(m_VkContext.getDevice(), static_cast<uint32_t>(WriteDescriptors.size()), WriteDescriptors.data(), 0, nullptr);
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> DescriptorSetLayoutSet(m_VkContext.getSwapChainImageSize(), m_pDeferredDescriptorSetLayout);

	VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
	DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorSetAllocateInfo.descriptorPool = m_pDescriptorPool;
	DescriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(m_VkContext.getSwapChainImageSize());
	DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayoutSet.data();

	m_DeferredDescriptorSet.resize(m_VkContext.getSwapChainImageSize());
	if (vkAllocateDescriptorSets(m_VkContext.getDevice(), &DescriptorSetAllocateInfo, m_DeferredDescriptorSet.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor set!");

	for (auto i = 0; i < m_VkContext.getSwapChainImageSize(); ++i)
	{
		VkDescriptorBufferInfo DescriptorBufferInfo = {};
		DescriptorBufferInfo.buffer = m_UniformBufferSet_OffScreen[i];
		DescriptorBufferInfo.offset = 0;
		DescriptorBufferInfo.range = sizeof(SUniformBufferObject_Deferred);

		VkDescriptorImageInfo DescriptorImageInfo_Position = {};
		DescriptorImageInfo_Position.imageView = m_PositionAttachment.getImageView();
		DescriptorImageInfo_Position.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImageInfo_Position.sampler = m_pSampler4DeferredRendering;

		VkDescriptorImageInfo DescriptorImageInfo_Normal = {};
		DescriptorImageInfo_Normal.imageView = m_NormalAttachment.getImageView();
		DescriptorImageInfo_Normal.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImageInfo_Normal.sampler = m_pSampler4DeferredRendering;

		VkDescriptorImageInfo DescriptorImageInfo_Color = {};
		DescriptorImageInfo_Color.imageView = m_ColorAttachment.getImageView();
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

		vkUpdateDescriptorSets(m_VkContext.getDevice(), static_cast<uint32_t>(WriteDescriptors.size()), WriteDescriptors.data(), 0, nullptr);
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createOffScreenCommandBuffers()
{
	m_OffScreenCommandBufferSet.resize(m_VkContext.getSwapChainImageSize());

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = m_VkContext.getCommandPool();
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_OffScreenCommandBufferSet.size());

	if (vkAllocateCommandBuffers(m_VkContext.getDevice(), &CommandBufferAllocateInfo, m_OffScreenCommandBufferSet.data()) != VK_SUCCESS)
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
		RenderPassBeginInfo.renderArea.extent = m_VkContext.getSwapchainExtent();

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
		m_pModel->draw(m_OffScreenCommandBufferSet[i], m_pOffScreenPipelineLayout, { static_cast<vk::DescriptorSet>(m_OffScreenDescriptorSet[i]) });
		vkCmdEndRenderPass(m_OffScreenCommandBufferSet[i]);

		if (vkEndCommandBuffer(m_OffScreenCommandBufferSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__createDeferredCommandBuffers()
{
	m_DeferredCommandBufferSet.resize(m_VkContext.getSwapChainImageSize());

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = m_VkContext.getCommandPool();
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_DeferredCommandBufferSet.size());

	if (vkAllocateCommandBuffers(m_VkContext.getDevice(), &CommandBufferAllocateInfo, m_DeferredCommandBufferSet.data()) != VK_SUCCESS)
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
		RenderPassBeginInfo.renderArea.extent = m_VkContext.getSwapchainExtent();

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
		if (vkCreateSemaphore(m_VkContext.getDevice(), &SemaphoreCreateInfo, nullptr, &m_OffScreenRenderingFinishedSemaphoreSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphores!");

		if (vkCreateSemaphore(m_VkContext.getDevice(), &SemaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphoreSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphores!");

		if (vkCreateSemaphore(m_VkContext.getDevice(), &SemaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphoreSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphores!");

		if (vkCreateFence(m_VkContext.getDevice(), &FenceCreateInfo, nullptr, &m_InFlightFenceSet[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create fences!");
	}
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__copyBuffer(VkBuffer vSrcBuffer, VkBuffer vDstBuffer, VkDeviceSize vBufferSize)
{
	auto CopyBuffer = [&](vk::CommandBuffer vCommandBuffer) {
		VkBufferCopy CopyRegion = {};
		CopyRegion.size = vBufferSize;
		CopyRegion.srcOffset = 0;
		CopyRegion.dstOffset = 0;
		vkCmdCopyBuffer(vCommandBuffer, vSrcBuffer, vDstBuffer, 1, &CopyRegion);
	};

	hiveVKT::executeImmediately(m_VkContext.getDevice(), m_VkContext.getCommandPool(), m_VkContext.getQueue(), CopyBuffer);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__updateUniformBuffer(uint32_t vImageIndex)
{
	//off-screen
	SUniformBufferObject_OffScreen UBO = {};

	UBO.Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
	UBO.View = this->fetchCamera()->getViewMatrix();
	UBO.Projection = this->fetchCamera()->getProjectionMatrix();

	void* Data = nullptr;
	vkMapMemory(m_VkContext.getDevice(), m_UniformBufferDeviceMemorySet_OffScreen[vImageIndex], 0, sizeof(UBO), 0, &Data);
	memcpy(Data, &UBO, sizeof(UBO));
	vkUnmapMemory(m_VkContext.getDevice(), m_UniformBufferDeviceMemorySet_OffScreen[vImageIndex]);

	//deferred
	SUniformBufferObject_Deferred UBO_Deferred = {};
	UBO_Deferred.ViewPosition = this->fetchCamera()->getPosition();

	vkMapMemory(m_VkContext.getDevice(), m_UniformBufferDeviceMemorySet_Deferred[vImageIndex], 0, sizeof(UBO_Deferred), 0, &Data);
	memcpy(Data, &UBO_Deferred, sizeof(UBO_Deferred));
	vkUnmapMemory(m_VkContext.getDevice(), m_UniformBufferDeviceMemorySet_Deferred[vImageIndex]);
}

//************************************************************************************
//Function:
void DeferredShading::CDeferredShadingApp::__loadModel()
{
	hiveVKT::SVertexLayout VertexLayout;
	VertexLayout.ComponentSet.push_back(hiveVKT::EVertexComponent::VERTEX_COMPONENT_POSITION);
	VertexLayout.ComponentSet.push_back(hiveVKT::EVertexComponent::VERTEX_COMPONENT_TEXCOORD);
	VertexLayout.ComponentSet.push_back(hiveVKT::EVertexComponent::VERTEX_COMPONENT_NORMAL);

	hiveVKT::STextureDescriptorBindingInfo TextureDescriptorBindingInfo;
	TextureDescriptorBindingInfo.TextureDescriptorBindingInfo.push_back({ hiveVKT::ETextureType::TEXTURE_TYPE_DIFF, 0 });
	TextureDescriptorBindingInfo.TextureDescriptorBindingInfo.push_back({ hiveVKT::ETextureType::TEXTURE_TYPE_SPEC, 1 });

	m_pModel = new hiveVKT::CModel();
	m_pModel->loadModel("../../resource/models/cyborg/cyborg.obj", VertexLayout, TextureDescriptorBindingInfo, m_VkContext.getDevice(), m_VkContext.getCommandPool(), m_VkContext.getQueue());
}


//************************************************************************************
//Function:
VkFormat DeferredShading::CDeferredShadingApp::__findSupportedFormat(const std::vector<VkFormat>& vCandidateFormatSet, VkImageTiling vImageTiling, VkFormatFeatureFlags vFormatFeatures)
{
	for (auto Format : vCandidateFormatSet)
	{
		VkFormatProperties FormatProperties;
		vkGetPhysicalDeviceFormatProperties(m_VkContext.getPhysicalDevice(), Format, &FormatProperties);

		if (vImageTiling == VK_IMAGE_TILING_LINEAR && ((FormatProperties.linearTilingFeatures&vFormatFeatures) == vFormatFeatures))
			return Format;

		else if (vImageTiling == VK_IMAGE_TILING_OPTIMAL && ((FormatProperties.optimalTilingFeatures&vFormatFeatures) == vFormatFeatures))
			return Format;
	}

	throw std::runtime_error("Failed to find supported format!");
}
