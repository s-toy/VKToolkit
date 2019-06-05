#pragma once
#include <vulkan/vulkan.hpp>
#include "VkDebugMessenger.h"
#include "Export.h"

#define Singleton(T) static T* getInstance() { static T Instance; return &Instance; }

namespace hiveVKT
{
	class VKT_DECLSPEC CVkContext
	{
	public:
		Singleton(CVkContext)

	public:
		~CVkContext();

		void setPreferDiscreteGpuHint(bool vPreferDiscreteGpuHint) { if(m_IsInitialized) return; m_PreferDiscreteGpuHint = vPreferDiscreteGpuHint; }
		void setForceGraphicsFunctionalityHint(bool vForceGraphicsFunctionalityHint) { if (m_IsInitialized) return; m_ForceGraphicsFunctionalityHint = vForceGraphicsFunctionalityHint; }
		void setForceComputeFunctionalityHint(bool vForceComputeFunctionalityHint) { if (m_IsInitialized) return; m_ForceComputeFunctionalityHint = vForceComputeFunctionalityHint; }
		void setForceTransferFunctionalityHint(bool vForceTransferFunctionalityHint) { if (m_IsInitialized) return; m_ForceTransferFunctionalityHint = vForceTransferFunctionalityHint; }
		void setEnableDebugUtilsHint(bool vEnableDebugUtilsHint) { if (m_IsInitialized) return; m_EnableDebugUtilsHint = vEnableDebugUtilsHint; }
		void setEnablePresentationHint(bool vEnablePresentationHint) { if (m_IsInitialized) return; m_EnablePresentationHint = vEnablePresentationHint; }
		void setEnableApiDumpHint(bool vEnableApiDumpHint) { if (m_IsInitialized) return; m_EnableApiDumpHint = vEnableApiDumpHint; }
		void setEnableFpsMonitorHint(bool vEnableFpsMonitorHint) { if (m_IsInitialized) return; m_EnableFpsMonitorHint = vEnableFpsMonitorHint; }
		void setEnableScreenshotHint(bool vEnableScreenshotHint) { if (m_IsInitialized) return; m_EnableScreenshotHint = vEnableScreenshotHint; }

		void setApplicationName(const std::string& vApplicationName) { if (m_IsInitialized) return; m_ApplicationName = vApplicationName; }
		void setEngineName(const std::string& vEngineName) { if (m_IsInitialized) return; m_EngineName = vEngineName; }
		void setApplicationVersion(uint32_t vApplicationVersion) { if (m_IsInitialized) return; m_ApplicationVersion = vApplicationVersion; }
		void setEngineVersion(uint32_t vEngineVersion) { if (m_IsInitialized) return; m_EngineVersion = vEngineVersion; }
		void setApiVersion(uint32_t vApiVersion) { if (m_IsInitialized) return; m_ApiVersion = vApiVersion; }

		//TODO：提供查询物理设备支持的扩展和特性
		void setEnabledPhysicalDeviceExtensions(const std::vector<std::string>& vEnabledDeviceExtensions) { if (m_IsInitialized) return; m_EnabledDeviceExtensions = vEnabledDeviceExtensions; }
		void setEnabledPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures& vEnabledPhysicalDeviceFeatures) { if (m_IsInitialized) return; m_EnabledPhysicalDeviceFeatures = vEnabledPhysicalDeviceFeatures; }

		std::vector<std::string> fetchEnabledDeviceExtensions() {
			return std::vector<std::string>();
		}//TODO
		std::vector<std::string> fetchEnabledDeviceLayers() {
			return std::vector<std::string>();
		}//TODO

		void createContext();
		void destroyContext();

		const vk::Instance& getVulkanInstance()const { return m_pInstance; }
		const vk::PhysicalDevice& getPhysicalDevice()const { return m_pPhysicalDevice; }
		const vk::DispatchLoaderDynamic& getDynamicDispatchLoader()const { return m_DynamicDispatchLoader; }
		const vk::Device& getVulkanDevice()const { return m_pDevice; }

		const CVkDebugUtilsMessenger& getDebugUtilsMessenger()const { return m_DebugUtilsMessenger; }

		int getComprehensiveQueueFamilyIndex()const { return std::get<0>(m_ComprehensiveQueue); }
		const vk::Queue& getComprehensiveQueue()const { return std::get<1>(m_ComprehensiveQueue); }
		const vk::CommandPool& getComprehensiveCommandPool()const { return std::get<2>(m_ComprehensiveQueue); }

		bool isContextCreated()const { return m_IsInitialized; }

	private:
		CVkContext();

		bool m_IsInitialized = false;

		bool m_PreferDiscreteGpuHint = false;
		bool m_ForceGraphicsFunctionalityHint = false;
		bool m_ForceComputeFunctionalityHint = false;
		bool m_ForceTransferFunctionalityHint = false;
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
		CVkDebugUtilsMessenger m_DebugUtilsMessenger;

		void __createVulkanInstance();
		void __createVulkanDevice();
		void __pickPhysicalDevice();
	};
}
