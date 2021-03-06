#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Export.h"

namespace hiveVKT
{
	class VKT_DECLSPEC CVkSwapchain
	{
	public:
		CVkSwapchain();
		~CVkSwapchain();

		bool createSwapchain(GLFWwindow* vGLFWwindow, vk::ImageUsageFlags vImageUsageFlags = vk::ImageUsageFlagBits::eColorAttachment, vk::SwapchainKHR vOldSwapchain = nullptr);
		void destroySwapchain();

		const vk::SurfaceKHR& getSurface()const { return m_pSurface; }
		const vk::SwapchainKHR& getSwapchain()const { return m_pSwapChain; }

		const vk::Image& getSwapchainImageAt(size_t vIndex)const
		{
			_ASSERT(m_IsInitialized && vIndex >= 0 && vIndex < m_SwapchainImages.size());
			return m_SwapchainImages[vIndex];
		}
		const vk::ImageView& getSwapchainImageViewAt(size_t vIndex)const
		{
			_ASSERT(m_IsInitialized && vIndex >= 0 && vIndex < m_SwapchainImages.size());
			return m_SwapchainImageViews[vIndex];
		}

		const vk::Format& getSwapchainImageFormat() const { return m_SwapchainImageFormat; }
		const vk::Extent2D& getSwapchainImageExtent() const { return m_SwapchainImageExtent; }

		size_t getNumSwapchainImage()const { return m_SwapchainImages.size(); }

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