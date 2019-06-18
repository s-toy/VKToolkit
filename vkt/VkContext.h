#pragma once
#include <vulkan/vulkan.hpp>
#include "VkDebugMessenger.h"
#include "Export.h"

#define Singleton(T) static T* getInstance() { static T Instance; return &Instance; }

namespace hiveVKT
{
	enum EContextExtraFunc
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
		void setExtraFuncStatus(uint32_t vStatus) { m_ExtraFuncStatus = vStatus; }
		bool getExtraFuncStatus(EContextExtraFunc vEnum) { return m_ExtraFuncStatus & vEnum; }

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

		uint32_t m_ExtraFuncStatus = EContextExtraFunc::UNDEFINED;

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
