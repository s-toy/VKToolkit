#pragma once
#include <vulkan/vulkan.hpp>

namespace hiveVKT
{
	struct SSwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR SurfaceCapabilities;
		std::vector<vk::SurfaceFormatKHR> SurfaceFormatSet;
		std::vector<vk::PresentModeKHR> PresentModeSet;
	};

	class CVkSwapChainCreator
	{
	public:
		CVkSwapChainCreator() { __init(); }

		vk::SwapchainKHR create(const vk::SurfaceKHR& vSurface, const vk::Device& vDevice, const vk::PhysicalDevice& vPhysicalDevice, int vWindowWidth, int vWindowHeight)
		{
			__prepareSwapchainCreateInfo(vSurface, vDevice, vPhysicalDevice, vWindowWidth, vWindowHeight);
			return vDevice.createSwapchainKHR(m_SwapchainCreateInfo);
		}

		vk::UniqueSwapchainKHR createUnique(const vk::SurfaceKHR& vSurface, const vk::Device& vDevice, const vk::PhysicalDevice& vPhysicalDevice, int vWindowWidth, int vWindowHeight)
		{
			__prepareSwapchainCreateInfo(vSurface, vDevice, vPhysicalDevice, vWindowWidth, vWindowHeight);
			return vDevice.createSwapchainKHRUnique(m_SwapchainCreateInfo);
		}

		SSwapChainSupportDetails queryPhysicalDeviceSwapChainSupport(vk::SurfaceKHR vSurface, const vk::PhysicalDevice& vPhysicalDevice)
		{
			_ASSERTE(vSurface != VK_NULL_HANDLE);

			SSwapChainSupportDetails SwapChainSupportDetails;
			SwapChainSupportDetails.SurfaceCapabilities = vPhysicalDevice.getSurfaceCapabilitiesKHR(vSurface);
			SwapChainSupportDetails.SurfaceFormatSet = vPhysicalDevice.getSurfaceFormatsKHR(vSurface);
			SwapChainSupportDetails.PresentModeSet = vPhysicalDevice.getSurfacePresentModesKHR(vSurface);

			return SwapChainSupportDetails;
		}

		vk::SwapchainCreateInfoKHR& fetchSwapchainCreateInfo() { return m_SwapchainCreateInfo; }

		vk::Format getSwapChainImageFormat() const { return m_SwapChainImageFormat; }
		vk::Extent2D getSwapChainExtent() const { return m_SwapChainExtent; }

	private:
		vk::SwapchainCreateInfoKHR m_SwapchainCreateInfo;
		vk::Format m_SwapChainImageFormat;
		vk::Extent2D m_SwapChainExtent;

		void __init()
		{
			m_SwapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
			m_SwapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
			m_SwapchainCreateInfo.imageArrayLayers = 1;
			m_SwapchainCreateInfo.queueFamilyIndexCount = 0;
			m_SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
			m_SwapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
			m_SwapchainCreateInfo.clipped = VK_TRUE;
		}

		void __prepareSwapchainCreateInfo(const vk::SurfaceKHR& vSurface, const vk::Device& vDevice, const vk::PhysicalDevice& vPhysicalDevice, int vWindowWidth, int vWindowHeight)
		{
			SSwapChainSupportDetails SwapChainSupportDetails = queryPhysicalDeviceSwapChainSupport(vSurface, vPhysicalDevice);
			vk::SurfaceFormatKHR SurfaceFormat = __determineSurfaceFormat(SwapChainSupportDetails.SurfaceFormatSet);
			vk::PresentModeKHR PresentMode = __determinePresentMode(SwapChainSupportDetails.PresentModeSet);
			VkExtent2D Extent = __determineSwapChainExtent(SwapChainSupportDetails.SurfaceCapabilities, vWindowWidth, vWindowHeight);
			uint32_t ImageCount = SwapChainSupportDetails.SurfaceCapabilities.minImageCount + 1;
			if (SwapChainSupportDetails.SurfaceCapabilities.maxImageCount > 0 && ImageCount > SwapChainSupportDetails.SurfaceCapabilities.maxImageCount)
				ImageCount = SwapChainSupportDetails.SurfaceCapabilities.maxImageCount;

			m_SwapChainImageFormat = SurfaceFormat.format;
			m_SwapChainExtent = Extent;
			m_SwapchainCreateInfo.surface = vSurface;
			m_SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
			m_SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
			m_SwapchainCreateInfo.imageExtent = Extent;
			m_SwapchainCreateInfo.minImageCount = ImageCount;
			m_SwapchainCreateInfo.presentMode = PresentMode;
			m_SwapchainCreateInfo.preTransform = SwapChainSupportDetails.SurfaceCapabilities.currentTransform;
		}

		vk::SurfaceFormatKHR __determineSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& vCandidateSurfaceFormatSet) const
		{
			if (vCandidateSurfaceFormatSet.size() == 1 && vCandidateSurfaceFormatSet[0].format == vk::Format::eUndefined)
				return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };

			for (const auto& Format : vCandidateSurfaceFormatSet)
			{
				if (Format.format == vk::Format::eB8G8R8A8Unorm && Format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
					return Format;
			}

			return vCandidateSurfaceFormatSet[0];
		}

		vk::PresentModeKHR __determinePresentMode(const std::vector<vk::PresentModeKHR>& vCandidatePresentModeSet) const
		{
			vk::PresentModeKHR BestPresentMode = vk::PresentModeKHR::eFifo;

			for (const auto& PresentMode : vCandidatePresentModeSet)
			{
				if (PresentMode == vk::PresentModeKHR::eMailbox)
					return PresentMode;
				else if (PresentMode == vk::PresentModeKHR::eImmediate)
					BestPresentMode = PresentMode;
			}

			return BestPresentMode;
		}

		VkExtent2D __determineSwapChainExtent(const vk::SurfaceCapabilitiesKHR& vSurfaceCapabilities, int vWindowWidth, int vWindowHeight) const
		{
			if (vSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return vSurfaceCapabilities.currentExtent;
			}
			else
			{
				VkExtent2D ActualExtent = { static_cast<uint32_t>(vWindowWidth), static_cast<uint32_t>(vWindowHeight) };

				ActualExtent.width = std::max(vSurfaceCapabilities.minImageExtent.width, std::min(vSurfaceCapabilities.maxImageExtent.width, ActualExtent.width));
				ActualExtent.height = std::max(vSurfaceCapabilities.minImageExtent.height, std::min(vSurfaceCapabilities.maxImageExtent.height, ActualExtent.height));

				return ActualExtent;
			}
		}
	};
}