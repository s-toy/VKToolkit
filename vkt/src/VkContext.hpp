#pragma once
#include <vector>
#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "VkSwapChainCreator.hpp"

namespace hiveVKT
{
	struct SQueueFamilyIndices
	{
		std::optional<uint32_t> QueueFamily;
		bool IsComplete() const { return QueueFamily.has_value(); }
	};

	class CVkDebugMessenger;

	class CVkContext
	{
	public:
		CVkContext();
		~CVkContext();

		bool initVulkan(GLFWwindow* vWindow, const std::vector<const char*>& vExtensions = { "VK_KHR_swapchain" });
		void destroyVulkan();

		vk::PhysicalDeviceFeatures& fetchPhysicalDeviceFeatures() { return m_VkPhysicalDeviceFeatures; }

		vk::Instance		getInstance() const { return m_VkInstance; }
		vk::SurfaceKHR		getSurface() const { return m_VkSurface; }
		vk::PhysicalDevice	getPhysicalDevice() const { return m_VkPhysicalDevice; }
		vk::Device			getDevice() const { return m_VkDevice; }
		vk::SwapchainKHR	getSwapchainKHR() const { return m_VkSwapchain; }
		vk::Format			getSwapchainImageFormat() const { return m_SwapChainImageFormat; }

		const vk::Extent2D&				getSwapchainExtent() const { return m_SwapChainExtent; }
		const SQueueFamilyIndices&		getRequiredQueueFamilyIndices() const { return m_RequiredQueueFamilyIndices; }
		const SSwapChainSupportDetails& getSwapChainSupportDetails() const { return m_SwapChainSupportDetails; }

	private:
		SQueueFamilyIndices m_RequiredQueueFamilyIndices = {};
		SSwapChainSupportDetails m_SwapChainSupportDetails = {};

		CVkDebugMessenger* m_pDebugMessenger = nullptr;

		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;

		vk::Instance m_VkInstance;
		vk::PhysicalDevice m_VkPhysicalDevice;
		vk::PhysicalDeviceFeatures m_VkPhysicalDeviceFeatures;
		vk::Device m_VkDevice;
		vk::SwapchainKHR m_VkSwapchain;
		vk::Format m_SwapChainImageFormat;
		vk::Extent2D m_SwapChainExtent;
		std::vector<vk::Image> m_SwapChainImages;
		std::vector<vk::ImageView> m_SwapChainImageViews;

		void __createInstance();
		void __createDebugMessenger();
		void __createSurface(GLFWwindow* vWindow);
		void __pickPhysicalDevice();
		void __createDevice();
		void __createSwapChain(int vWidth, int vHeight);
		void __retrieveSwapChainImages();
		void __createImageViews();
		void __retrieveDeviceQueues();

		SQueueFamilyIndices __findRequiredQueueFamilies(const vk::PhysicalDevice& vPhysicalDevice);
	};
}