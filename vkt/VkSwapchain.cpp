#include "VkSwapchain.h"
#include "VkContext.h"

hiveVKT::CVkSwapchain::CVkSwapchain()
{
}

hiveVKT::CVkSwapchain::~CVkSwapchain()
{
}

//*****************************************************************************************
//FUNCTION:
bool hiveVKT::CVkSwapchain::createSwapchain(GLFWwindow* vGLFWwindow, vk::ImageUsageFlags vImageUsageFlags, vk::SwapchainKHR vOldSwapchain)
{
	if (m_IsInitialized) return true;
	if (!vGLFWwindow) return false;

	m_pGLFWwindow = vGLFWwindow;

	if (!CVkContext::getInstance()->isContextCreated()) return false;
	glfwCreateWindowSurface(CVkContext::getInstance()->getVulkanInstance(), vGLFWwindow, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_pSurface));
	if (!m_pSurface) return false;

	uint32_t ComprehensiveQueueFamilyIndex = CVkContext::getInstance()->getComprehensiveQueueFamilyIndex();
	bool PresentationSupport = CVkContext::getInstance()->getPhysicalDevice().getSurfaceSupportKHR(ComprehensiveQueueFamilyIndex, m_pSurface);
	if (!PresentationSupport) return false;

	vk::SurfaceCapabilitiesKHR SurfaceCapabilities = CVkContext::getInstance()->getPhysicalDevice().getSurfaceCapabilitiesKHR(m_pSurface);

	vk::ImageUsageFlags SupportedUsage = SurfaceCapabilities.supportedUsageFlags;
	SupportedUsage = ~SupportedUsage;
	if (SupportedUsage & vImageUsageFlags) return false;

	vk::CompositeAlphaFlagBitsKHR CompositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	if (!(CompositeAlpha & SurfaceCapabilities.supportedCompositeAlpha)) return false; //TODO

	vk::Extent2D Extent = SurfaceCapabilities.currentExtent;
	uint32_t ImageCount = SurfaceCapabilities.minImageCount + 1;
	if (SurfaceCapabilities.maxImageCount > 0 && ImageCount > SurfaceCapabilities.maxImageCount)
		ImageCount = SurfaceCapabilities.maxImageCount;

	std::vector<vk::SurfaceFormatKHR> SurfaceFormats = CVkContext::getInstance()->getPhysicalDevice().getSurfaceFormatsKHR(m_pSurface);
	std::vector<vk::PresentModeKHR> PresentModes = CVkContext::getInstance()->getPhysicalDevice().getSurfacePresentModesKHR(m_pSurface);

	vk::SurfaceFormatKHR SurfaceFormat = SurfaceFormats[0];
	if (SurfaceFormats.size() == 1 && SurfaceFormats[0].format == vk::Format::eUndefined) SurfaceFormat = { vk::Format::eB8G8R8A8Unorm,vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear };
	else
	{
		for (const auto& Format : SurfaceFormats)
			if (Format.format == vk::Format::eB8G8R8A8Unorm && Format.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
				SurfaceFormat = Format;
	}
	vk::PresentModeKHR PresentMode = PresentModes[0];
	for (const auto& Mode : PresentModes)
	{
		if (Mode == vk::PresentModeKHR::eMailbox) { PresentMode = Mode; break; }
		else if (Mode == vk::PresentModeKHR::eImmediate) PresentMode = Mode;
	}

	vk::SwapchainCreateInfoKHR SwapchainCreateInfo = {
		vk::SwapchainCreateFlagsKHR(),
		m_pSurface,
		ImageCount,SurfaceFormat.format,SurfaceFormat.colorSpace,Extent,1,vImageUsageFlags,
		vk::SharingMode::eExclusive,0,nullptr,
		SurfaceCapabilities.currentTransform,
		CompositeAlpha,
		PresentMode,
		true,
		vOldSwapchain
	};

	m_pSwapChain = CVkContext::getInstance()->getVulkanDevice().createSwapchainKHR(SwapchainCreateInfo); //TODO

	m_SwapchainImages = CVkContext::getInstance()->getVulkanDevice().getSwapchainImagesKHR(m_pSwapChain);

	for (auto& Image : m_SwapchainImages)
	{
		vk::ImageViewCreateInfo ImageViewCreateInfo = {
			vk::ImageViewCreateFlags(),
			Image,
			vk::ImageViewType::e2D,SurfaceFormat.format,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor,0,1,0,1}
		};

		m_SwapchainImageViews.emplace_back(CVkContext::getInstance()->getVulkanDevice().createImageView(ImageViewCreateInfo)); //TODO
	}

	m_SwapchainImageFormat = SurfaceFormat.format;
	m_SwapchainImageExtent = Extent;

	m_IsInitialized = true;

	return true;
}

//*****************************************************************************************
//FUNCTION:
void hiveVKT::CVkSwapchain::destroySwapchain()
{
	if (!m_IsInitialized)return;
	if (!CVkContext::getInstance()->isContextCreated())
	{
		//TODO£ºOutput warning to info user that swap chain should be destroyed before context
		return;
	}

	for (auto& ImageView : m_SwapchainImageViews)
		CVkContext::getInstance()->getVulkanDevice().destroyImageView(ImageView);

	CVkContext::getInstance()->getVulkanDevice().destroySwapchainKHR(m_pSwapChain);
	CVkContext::getInstance()->getVulkanInstance().destroySurfaceKHR(m_pSurface);

	m_IsInitialized = false;
}
