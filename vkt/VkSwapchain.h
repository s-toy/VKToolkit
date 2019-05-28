#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace hiveVKT
{
	class CVkSwapchain
	{
	public:
		CVkSwapchain();
		~CVkSwapchain();

		void createSwapchain(GLFWwindow* vGLFWwindow, vk::ImageUsageFlags vImageUsageFlags = vk::ImageUsageFlagBits::eColorAttachment, vk::SwapchainKHR vOldSwapchain = nullptr);
		void destroySwapchain();

		const vk::SurfaceKHR& getSurface()const { _ASSERT(m_IsInitialized); return m_pSurface; }
		const vk::SwapchainKHR& getSwapchain()const { _ASSERT(m_IsInitialized); return m_pSwapChain; }

		const vk::Image& getSwapchainImageAt(int vIndex)const { _ASSERT(m_IsInitialized && vIndex >= 0 && vIndex < m_SwapchainImages.size()); return m_SwapchainImages[vIndex]; }
		const vk::ImageView& getSwapchainImageViewAt(int vIndex)const { _ASSERT(m_IsInitialized && vIndex >= 0 && vIndex < m_SwapchainImages.size()); return m_SwapchainImageViews[vIndex]; }

		size_t getNumSwapchainImage()const { _ASSERT(m_IsInitialized); return m_SwapchainImages.size(); }

	private:
		bool m_IsInitialized = false;

		GLFWwindow* m_pGLFWwindow = nullptr;

		vk::SurfaceKHR m_pSurface = nullptr;
		vk::SwapchainKHR m_pSwapChain = nullptr;

		std::vector<vk::Image> m_SwapchainImages = {};
		std::vector<vk::ImageView> m_SwapchainImageViews = {};

		vk::Format m_SwapchainImageFormat = vk::Format::eUndefined;
		vk::Extent2D m_SwapchainImageExtent = vk::Extent2D{};
	};
}