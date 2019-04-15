#pragma once
#include <optional>
#include <functional>
#include <vector>
#include <GLFW/glfw3.h>
#include "Common.hpp"
#include "VkContext.hpp"

namespace hiveVKT
{
	class CCamera;

	class CVkApplicationBase
	{
	public:
		CVkApplicationBase() = default;
		virtual ~CVkApplicationBase() = default;

		void run();

		void setWindowSize(int vWidth, int vHeight) { m_WindowCreateInfo.WindowWidth = vWidth; m_WindowCreateInfo.WindowHeight = vHeight; }
		void setWindowPos(int vPosX, int vPosY) { m_WindowCreateInfo.WindowPosX = vPosX; m_WindowCreateInfo.WindowPosY = vPosY; }
		void setWindowFullScreen(bool vFullScreen) { m_WindowCreateInfo.IsWindowFullScreen = vFullScreen; }
		void setWindowResizable(bool vResizable) { m_WindowCreateInfo.IsWindowResizable = vResizable; }
		void setWindowTitle(const std::string& vTitle) { m_WindowCreateInfo.WindowTitle = vTitle; }

		CCamera*	fetchCamera() const { return m_pCamera; }
		GLFWwindow* fetchWindow() const { return m_pWindow; }
		CVkContext&	fetchVkContext() { return m_VkContext; }

		SViewInfo getViewInfo() const;

		double getFrameInterval() const { return m_FrameInterval; }

		vk::PhysicalDeviceFeatures& fetchPhysicalDeviceFeatures() { return m_VkContext.fetchPhysicalDeviceFeatures(); }

		void registerUpdateFunction(std::function<void()> vFunc) { m_UpdateFunctionSet.emplace_back(vFunc); }

	protected:
		_DISALLOW_COPY_AND_ASSIGN(CVkApplicationBase);

		virtual bool _initV();
		virtual bool _renderV();
		virtual bool _isRenderLoopDoneV();
		virtual void _handleEventV() {}
		virtual void _destroyV();

		vk::Instance _instance()				const { return m_VkContext.getInstance(); }
		vk::SurfaceKHR _surface()				const { return m_VkContext.getSurface(); }
		vk::PhysicalDevice _physicalDevice()	const { return m_VkContext.getPhysicalDevice(); }
		vk::Device _device()					const { return m_VkContext.getDevice(); }
		vk::SwapchainKHR _swapchain()			const { return m_VkContext.getSwapchainKHR(); }
		vk::Format _swapchainImageFormat()		const { return m_VkContext.getSwapchainImageFormat(); }
		vk::Extent2D _swapchainExtent()			const { return m_VkContext.getSwapchainExtent(); }

		const SQueueFamilyIndices& _requiredQueueFamilyIndices() const { return m_VkContext.getRequiredQueueFamilyIndices(); }
		const SSwapChainSupportDetails& _swapChainSupportDetails() const { return m_VkContext.getSwapChainSupportDetails(); }

	private:
		CVkContext m_VkContext;

		GLFWwindow* m_pWindow = nullptr;
		CCamera*	m_pCamera = nullptr;

		SWindowCreateInfo m_WindowCreateInfo = {};

		std::vector<std::function<void()>> m_UpdateFunctionSet;

		double	m_FrameInterval = 0.0;
		bool	m_IsInitialized = false;
		bool	m_IsRenderLoopDone = false;

		bool __initWindow();
	};
}