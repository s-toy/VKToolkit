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
		CVkSwapChainCreator();

		vk::SwapchainKHR create(const vk::SurfaceKHR& vSurface, const vk::Device& vDevice, const vk::PhysicalDevice& vPhysicalDevice, int vWindowWidth, int vWindowHeight);
		vk::UniqueSwapchainKHR createUnique(const vk::SurfaceKHR& vSurface, const vk::Device& vDevice, const vk::PhysicalDevice& vPhysicalDevice, int vWindowWidth, int vWindowHeight);

		SSwapChainSupportDetails queryPhysicalDeviceSwapChainSupport(vk::SurfaceKHR vSurface, const vk::PhysicalDevice& vPhysicalDevice);
		vk::SwapchainCreateInfoKHR& fetchSwapchainCreateInfo() { return m_SwapchainCreateInfo; }

		vk::Format getSwapChainImageFormat() const { return m_SwapChainImageFormat; }
		vk::Extent2D getSwapChainExtent() const { return m_SwapChainExtent; }

	private:
		vk::SwapchainCreateInfoKHR m_SwapchainCreateInfo;
		vk::Format m_SwapChainImageFormat;
		vk::Extent2D m_SwapChainExtent;

		void __init();
		void __prepareSwapchainCreateInfo(const vk::SurfaceKHR& vSurface, const vk::Device& vDevice, const vk::PhysicalDevice& vPhysicalDevice, int vWindowWidth, int vWindowHeight);

		vk::SurfaceFormatKHR __determineSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& vCandidateSurfaceFormatSet) const;
		vk::PresentModeKHR __determinePresentMode(const std::vector<vk::PresentModeKHR>& vCandidatePresentModeSet) const;
		VkExtent2D __determineSwapChainExtent(const vk::SurfaceCapabilitiesKHR& vSurfaceCapabilities, int vWindowWidth, int vWindowHeight) const;
	};
}