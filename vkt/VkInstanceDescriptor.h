#pragma once
#include <set>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Common.h"

namespace hiveVKT
{
	class CVkInstanceDescriptor
	{
	public:
		CVkInstanceDescriptor();
		~CVkInstanceDescriptor();

		void setApplicationName(const char* vApplicationName) { m_ApplicationInfo.pApplicationName = vApplicationName; }
		void setApplicationVersion(uint32_t vVersion) { m_ApplicationInfo.applicationVersion = vVersion; }
		void setEngineName(const char* vEngineName) { m_ApplicationInfo.pEngineName = vEngineName; }
		void setEngineVersion(uint32_t vVersion) { m_ApplicationInfo.engineVersion = vVersion; }
		void setApiVersion(uint32_t vVersion) { m_ApplicationInfo.apiVersion = vVersion; }

		void addLayer(const char* vLayer) { m_InstanceLayerSet.emplace_back(vLayer); }
		void addExtension(const char* vExtension) { m_InstanceExtensionSet.emplace_back(vExtension); }
		void setEnabledLayers(const std::vector<const char*> vEnabledLayers) { m_InstanceLayerSet = vEnabledLayers; }
		void setEnabledExtensions(const std::vector<const char*> vEnabledExtensions) { m_InstanceExtensionSet = vEnabledExtensions; }

	protected:
		bool _isValid() const { return true; } //TODO:检查有效性
		const vk::InstanceCreateInfo& _getInstanceCreateInfo() { __assemblingInstanceCreateInfo(); return m_InstanceCreateInfo; }

	private:
		vk::InstanceCreateInfo m_InstanceCreateInfo;
		vk::ApplicationInfo m_ApplicationInfo;
		std::vector<const char*> m_InstanceLayerSet;
		std::vector<const char*> m_InstanceExtensionSet;

		void __assemblingInstanceCreateInfo();
		bool __checkInstanceLayersAndExtensionsSupport() const;

		friend class CVkObjectCreator;
	};
}