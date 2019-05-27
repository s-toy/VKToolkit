#pragma once
#include <set>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Common.h"

namespace hiveVKT
{
	class CVkInstanceCreator
	{
	public:
		CVkInstanceCreator();

		vk::Instance create();
		vk::UniqueInstance createUnique();

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

		void __init();
		void __prepareInstanceCreateInfo();
		void __addDebugLayersAndExtensionsIfNecessary();
		bool __checkInstanceLayersAndExtensionsSupport() const;
	};
}