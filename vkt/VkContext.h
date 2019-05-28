#pragma once
#include <vulkan/vulkan.hpp>

#define Singleton(T) static T* getInstance() { static T Instance; return &Instance; }

namespace hiveVKT
{
	class CVkContext
	{
	public:
		Singleton(CVkContext)

	public:
		~CVkContext();

		void setForceGraphicsFunctionalityHint(bool vForceGraphicsFunctionalityHint) { _ASSERT(!m_IsInitialized); m_ForceGraphicsFunctionalityHint = vForceGraphicsFunctionalityHint; }
		void setEnableDebugUtilsHint(bool vEnableDebugUtilsHint) { _ASSERT(!m_IsInitialized); m_EnableDebugUtilsHint = vEnableDebugUtilsHint; }
		void setEnablePresentationHint(bool vInitPresentationExtensionsHint) { _ASSERT(!m_IsInitialized); m_EnablePresentationHint = vInitPresentationExtensionsHint; }
		void setEnableApiDumpHint(bool vEnableApiDumpHint) { _ASSERT(!m_IsInitialized); m_EnableApiDumpHint = vEnableApiDumpHint; }
		void setEnableFpsMonitorHint(bool vEnableFpsMonitorHint) { _ASSERT(!m_IsInitialized); m_EnableFpsMonitorHint = vEnableFpsMonitorHint; }
		void setEnableScreenshotHint(bool vEnableScreenshotHint) { _ASSERT(!m_IsInitialized); m_EnableScreenshotHint = vEnableScreenshotHint; }

		void setApplicationName(const std::string& vApplicationName) { _ASSERT(!m_IsInitialized); m_ApplicationName = vApplicationName; }
		void setEngineName(const std::string& vEngineName) { _ASSERT(!m_IsInitialized); m_EngineName = vEngineName; }
		void setApplicationVersion(uint32_t vApplicationVersion) { _ASSERT(!m_IsInitialized); m_ApplicationVersion = vApplicationVersion; }
		void setEngineVersion(uint32_t vEngineVersion) { _ASSERT(!m_IsInitialized); m_EngineVersion = vEngineVersion; }
		void setApiVersion(uint32_t vApiVersion) { _ASSERT(!m_IsInitialized); m_ApiVersion = vApiVersion; }

		void setEnabledPhysicalDeviceExtensions(const std::vector<std::string>& vEnabledDeviceExtensions) { _ASSERT(!m_IsInitialized); m_EnabledDeviceExtensions = vEnabledDeviceExtensions; }
		void setEnabledPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures& vEnabledPhysicalDeviceFeatures) { _ASSERT(!m_IsInitialized); m_EnabledPhysicalDeviceFeatures = vEnabledPhysicalDeviceFeatures; }

		void createContext(uint32_t vPhysicalDeviceID = 0);
		void destroyContext();

		const vk::Instance& getVulkanInstance()const { _ASSERT(m_IsInitialized); return m_pInstance; }
		const vk::PhysicalDevice& getPhysicalDevice()const { _ASSERT(m_IsInitialized); return m_pPhysicalDevice; }
		const vk::DispatchLoaderDynamic& getDynamicDispatchLoader()const { _ASSERT(m_IsInitialized); return m_DynamicDispatchLoader; }
		const vk::Device& getVulkanDevice()const { _ASSERT(m_IsInitialized); return m_pDevice; }

		int getComprehensiveQueueFamilyIndex()const { _ASSERT(m_IsInitialized); return std::get<0>(m_ComprehensiveQueue); }
		const vk::Queue& getComprehensiveQueue()const { _ASSERT(m_IsInitialized); return std::get<1>(m_ComprehensiveQueue); }
		const vk::CommandPool& getComprehensiveCommandPool()const { _ASSERT(m_IsInitialized); return std::get<2>(m_ComprehensiveQueue); }

		bool isContextCreated()const { return m_IsInitialized; }

	private:
		CVkContext();

		bool m_IsInitialized = false;

		bool m_ForceGraphicsFunctionalityHint = false;
		bool m_EnableDebugUtilsHint = false;
		bool m_EnablePresentationHint = false;
		bool m_EnableApiDumpHint = false;
		bool m_EnableFpsMonitorHint = false;
		bool m_EnableScreenshotHint = false;

		std::string m_ApplicationName = "Application";
		std::string m_EngineName = "HiveVKT";
		uint32_t m_ApplicationVersion = VK_MAKE_VERSION(0, 0, 0);
		uint32_t m_EngineVersion = VK_MAKE_VERSION(0, 0, 0);
		uint32_t m_ApiVersion = VK_API_VERSION_1_0;

		std::vector<std::string> m_EnabledInstanceLayers = {};
		std::vector<std::string> m_EnabledInstanceExtensions = {};
		std::vector<std::string> m_EnabledDeviceExtensions = {};
		vk::PhysicalDeviceFeatures m_EnabledPhysicalDeviceFeatures = {};

		vk::Instance m_pInstance = nullptr;
		vk::PhysicalDevice m_pPhysicalDevice = nullptr;
		vk::DispatchLoaderDynamic m_DynamicDispatchLoader;
		vk::Device m_pDevice = nullptr;
		std::tuple<uint32_t, vk::Queue, vk::CommandPool> m_ComprehensiveQueue = { UINT32_MAX,nullptr,nullptr }; //<queue family index, queue, command pool>

		void __createVulkanInstance();
		void __createVulkanDevice(uint32_t vPhysicalDeviceID);
		void __determineComprehensiveQueueFamilyIndex();
	};
}
