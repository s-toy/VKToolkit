#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Common.hpp"
#include "VKTExport.hpp"
#include "VkSwapChainCreator.hpp"

namespace hiveVKT
{
	class CVkDebugMessenger;

	struct SQueueFamilyIndices
	{
		std::optional<uint32_t> QueueFamily;
		bool IsComplete() { return QueueFamily.has_value(); }
	};

	class VKT_DECLSPEC CVkApplicationBase
	{
	public:
		CVkApplicationBase() = default;
		virtual ~CVkApplicationBase() = default;

		void run();

		void setWindowSize(int vWidth, int vHeight) { m_DisplayInfo.WindowWidth = vWidth; m_DisplayInfo.WindowHeight = vHeight; }
		void setWindowPos(int vPosX, int vPosY) { m_DisplayInfo.WindowPosX = vPosX; m_DisplayInfo.WindowPosY = vPosY; }
		void setWindowFullScreen(bool vFullScreen) { m_DisplayInfo.IsWindowFullScreen = vFullScreen; }
		void setWindowResizable(bool vResizable) { m_DisplayInfo.IsWindowResizable = vResizable; }
		void setWindowTitle(const std::string& vTitle) { m_DisplayInfo.WindowTitle = vTitle; }

		vk::PhysicalDeviceFeatures& fetchPhysicalDeviceFeatures() { return m_VkPhysicalDeviceFeatures; }

	protected:
		_DISALLOW_COPY_AND_ASSIGN(CVkApplicationBase);

		virtual bool _initV();
		virtual bool _renderV();
		virtual bool _isRenderLoopDoneV();
		virtual void _handleEventV() {}
		virtual void _destroyV();

		GLFWwindow* _window() const { return m_pWindow; }

		vk::Instance _instance()				const { return m_VkInstance; }
		vk::SurfaceKHR _surface()				const { return m_VkSurface; }
		vk::PhysicalDevice _physicalDevice()	const { return m_VkPhysicalDevice; }
		vk::Device _device()					const { return m_VkDevice; }
		vk::SwapchainKHR _swapchain()			const { return m_VkSwapchain; }
		vk::Format _swapchainImageFormat()		const { return m_SwapChainImageFormat; }
		vk::Extent2D _swapchainExtent()			const { return m_SwapChainExtent; }

		const SQueueFamilyIndices& _requiredQueueFamilyIndices() const { return m_RequiredQueueFamilyIndices; }
		const SSwapChainSupportDetails& _swapChainSupportDetails() const { return m_SwapChainSupportDetails; }

	private:
		CVkDebugMessenger* m_pDebugMessenger = nullptr;
		GLFWwindow* m_pWindow = nullptr;

		SDisplayInfo m_DisplayInfo = {};
		SQueueFamilyIndices m_RequiredQueueFamilyIndices = {};
		SSwapChainSupportDetails m_SwapChainSupportDetails = {};

		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;

		vk::Instance m_VkInstance;
		vk::PhysicalDevice m_VkPhysicalDevice;
		vk::PhysicalDeviceFeatures m_VkPhysicalDeviceFeatures;
		vk::Device m_VkDevice;
		vk::SwapchainKHR m_VkSwapchain;
		vk::Format m_SwapChainImageFormat;
		vk::Extent2D m_SwapChainExtent;

		bool m_IsInitialized = false;
		bool m_IsRenderLoopDone = false;

		bool __initWindow();
		bool __initVulkan();

		void __createInstance();
		void __createDebugMessenger();
		void __createSurface();
		void __pickPhysicalDevice();
		void __createDevice();
		void __createSwapChain();

		SQueueFamilyIndices __findRequiredQueueFamilies(const vk::PhysicalDevice& vPhysicalDevice);
	};
}