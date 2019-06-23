#pragma once
#include <vulkan/vulkan.hpp>
#include "VkDebugMessenger.h"
#include "Export.h"

namespace hiveVKT
{
	enum EContextFeature
	{
		UNDEFINED = 0,
		PREFER_DISCRETE_GPU = 1,
		FORCE_GARPHICS_FUNCTIONALITY = 2,
		FORCE_COMPUTE_FUNCTIONALITY = 4,
		FORCE_TRANSFER_FUNCTIONALITY = 8,
		ENABLE_DEBUG_UTILS = 16,
		ENABLE_PRESENTATION = 32,
		ENABLE_API_DUMP = 64,
		ENABLE_FPS_MONITOR = 128,
		ENABLE_SCREENSHOT = 256
	};

	class VKT_DECLSPEC CVkContext
	{
	public:
		Singleton(CVkContext)

	public:
		~CVkContext();
		void enableContextFeature(uint32_t vFeatureSet) { m_FeatureStatus |= vFeatureSet; }
		void disableAllContextFeature() { m_FeatureStatus = EContextFeature::UNDEFINED; }

		bool isFeatureEnabled(EContextFeature vFeature) { return m_FeatureStatus & vFeature; }

		void setApplicationName(const std::string& vApplicationName) { if (m_IsInitialized) return; m_ApplicationName = vApplicationName; }

		//TODO：提供查询物理设备支持的扩展和特性
		void setEnabledPhysicalDeviceExtensions(const std::vector<std::string>& vEnabledDeviceExtensions) { if (m_IsInitialized) return; m_EnabledDeviceExtensions = vEnabledDeviceExtensions; }
		void setEnabledPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures& vEnabledPhysicalDeviceFeatures) { if (m_IsInitialized) return; m_EnabledPhysicalDeviceFeatures = vEnabledPhysicalDeviceFeatures; }

		void createContext();
		void destroyContext();

		const vk::Instance& getVulkanInstance()const { return m_pInstance; }
		const vk::PhysicalDevice& getPhysicalDevice()const { return m_pPhysicalDevice; }
		const vk::DispatchLoaderDynamic& getDynamicDispatchLoader()const { return m_DynamicDispatchLoader; }
		const vk::Device& getVulkanDevice()const { return m_pDevice; }

		uint32_t getWarningAndErrorCount() const { return m_DebugUtilsMessenger.getWarningAndErrorCount(); }
		uint32_t getWarningCount() const { return m_DebugUtilsMessenger.getWarningCount(); }
		uint32_t getErrorCount() const { return m_DebugUtilsMessenger.getErrorCount(); }

		int getComprehensiveQueueFamilyIndex()const { return std::get<0>(m_ComprehensiveQueue); }
		const vk::Queue& getComprehensiveQueue()const { return std::get<1>(m_ComprehensiveQueue); }
		const vk::CommandPool& getComprehensiveCommandPool()const { return std::get<2>(m_ComprehensiveQueue); }

		bool isContextCreated()const { return m_IsInitialized; }

	private:
		CVkContext();

		bool m_IsInitialized = false;

		uint32_t m_FeatureStatus = EContextFeature::UNDEFINED;

		std::string m_ApplicationName = "Application";

		std::vector<std::string> m_EnabledDeviceExtensions = {};
		vk::PhysicalDeviceFeatures m_EnabledPhysicalDeviceFeatures = {};

		vk::Instance m_pInstance = nullptr;
		vk::PhysicalDevice m_pPhysicalDevice = nullptr;
		vk::DispatchLoaderDynamic m_DynamicDispatchLoader;
		vk::Device m_pDevice = nullptr;
		std::tuple<uint32_t, vk::Queue, vk::CommandPool> m_ComprehensiveQueue = { UINT32_MAX,nullptr,nullptr }; //<queue family index, queue, command pool>
		CVkDebugUtilsMessenger m_DebugUtilsMessenger;

		void __createVulkanInstance();
		void __createVulkanDevice();
		void __pickPhysicalDevice();
	};
}
