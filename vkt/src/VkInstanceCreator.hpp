#pragma once
#include <set>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Common.hpp"

namespace hiveVKT
{
	class CVkInstanceCreator
	{
	public:
		CVkInstanceCreator()
		{
			__init();
		}

		vk::Instance create()
		{
			__prepareInstanceCreateInfo();
			return vk::createInstance(m_InstanceCreateInfo, nullptr);
		}

		vk::UniqueInstance createUnique()
		{
			__prepareInstanceCreateInfo();
			return vk::createInstanceUnique(m_InstanceCreateInfo, nullptr);
		}

		void setApplicationName(const char* vApplicationName) { m_ApplicationInfo.pApplicationName = vApplicationName; }
		void setApplicationVersion(uint32_t vVersion) { m_ApplicationInfo.applicationVersion = vVersion; }
		void setEngineName(const char* vEngineName) { m_ApplicationInfo.pEngineName = vEngineName; }
		void setEngineVersion(uint32_t vVersion) { m_ApplicationInfo.engineVersion = vVersion; }
		void setApiVersion(uint32_t vVersion) { m_ApplicationInfo.apiVersion = vVersion; }

		void addLayer(const char* vLayer) { m_InstanceLayerSet.emplace_back(vLayer); }
		void addExtension(const char* vExtension) { m_InstanceExtensionSet.emplace_back(vExtension); }
		void setEnabledLayers(const std::vector<const char*> vEnabledLayers) { m_InstanceLayerSet = vEnabledLayers; }
		void setEnabledExtensions(const std::vector<const char*> vEnabledExtensions) { m_InstanceExtensionSet = vEnabledExtensions; }

	private:
		vk::InstanceCreateInfo m_InstanceCreateInfo;
		vk::ApplicationInfo m_ApplicationInfo;
		std::vector<const char*> m_InstanceLayerSet;
		std::vector<const char*> m_InstanceExtensionSet;

		void __init()
		{
			m_ApplicationInfo.pApplicationName = "hiveApplication";
			m_ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			m_ApplicationInfo.pEngineName = "hiveVKT";
			m_ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			m_ApplicationInfo.apiVersion = VK_API_VERSION_1_1;
		}

		void __prepareInstanceCreateInfo()
		{
			__addDebugLayersAndExtensionsIfNecessary();

#ifdef _ENABLE_DEBUG_UTILS
			if (!__checkInstanceLayersAndExtensionsSupport())
				throw std::runtime_error("Not all requested instance layers and extensions are available.");
#endif

			m_InstanceCreateInfo.pApplicationInfo = &m_ApplicationInfo;
			m_InstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensionSet.size());
			m_InstanceCreateInfo.ppEnabledExtensionNames = m_InstanceExtensionSet.data();
			m_InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_InstanceLayerSet.size());
			m_InstanceCreateInfo.ppEnabledLayerNames = m_InstanceLayerSet.data();
		}

		void __addDebugLayersAndExtensionsIfNecessary()
		{
#ifdef _ENABLE_DEBUG_UTILS
			m_InstanceLayerSet.emplace_back("VK_LAYER_LUNARG_standard_validation");
			m_InstanceExtensionSet.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
		}

		bool __checkInstanceLayersAndExtensionsSupport()const
		{
			auto InstanceLayerPropertiesSet = vk::enumerateInstanceLayerProperties();
			auto InstanceExtensionPropertiesSet = vk::enumerateInstanceExtensionProperties();

			std::set<std::string> RequiredInstanceLayerSet(m_InstanceLayerSet.begin(), m_InstanceLayerSet.end());
			std::set<std::string> RequiredInstanceExtensionSet(m_InstanceExtensionSet.begin(), m_InstanceExtensionSet.end());
			for (const auto& LayerProperty : InstanceLayerPropertiesSet) RequiredInstanceLayerSet.erase(LayerProperty.layerName);
			for (const auto& ExtensionProperty : InstanceExtensionPropertiesSet) RequiredInstanceExtensionSet.erase(ExtensionProperty.extensionName);

			return RequiredInstanceLayerSet.empty() && RequiredInstanceExtensionSet.empty();
		}
	};
}