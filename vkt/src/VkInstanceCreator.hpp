#pragma once
#include <set>
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
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
		void defaultLayersAndExtensions(bool vEnable) { m_EnableDefaultLayersAndExtensions = vEnable; }

	private:
		vk::InstanceCreateInfo m_InstanceCreateInfo;
		vk::ApplicationInfo m_ApplicationInfo;
		std::vector<const char *> m_InstanceLayerSet;
		std::vector<const char *> m_InstanceExtensionSet;

		bool m_EnableDefaultLayersAndExtensions = true;

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
			__addDefaultLayersAndExtensionsIfNecessary();

			m_InstanceCreateInfo.pApplicationInfo = &m_ApplicationInfo;
			m_InstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensionSet.size());
			m_InstanceCreateInfo.ppEnabledExtensionNames = m_InstanceExtensionSet.data();
			m_InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_InstanceLayerSet.size());
			m_InstanceCreateInfo.ppEnabledLayerNames = m_InstanceLayerSet.data();
		}

		void __addDefaultLayersAndExtensionsIfNecessary()
		{
			if (!m_EnableDefaultLayersAndExtensions) return;

			uint32_t GLFWExtensionCount = 0;

			//HACK: glfw functions should not be called inside this class.
			const char** pGLFWExtensions = glfwGetRequiredInstanceExtensions(&GLFWExtensionCount);
			_ASSERT_EXPR(pGLFWExtensions, "glfwGetRequiredInstanceExtensions() failed, make sure that glfwInit() has been called before!");

			auto Extensions = std::vector<const char*>(pGLFWExtensions, pGLFWExtensions + GLFWExtensionCount);
			m_InstanceExtensionSet.insert(m_InstanceExtensionSet.end(), Extensions.begin(), Extensions.end());
			_ASSERT(!m_InstanceExtensionSet.empty());

		#ifdef _ENABLE_DEBUG_UTILS
			m_InstanceLayerSet.emplace_back("VK_LAYER_LUNARG_standard_validation");
			m_InstanceExtensionSet.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			if (!__checkInstanceLayersSupport()) _THROW_RUNTINE_ERROR("Not all requested instance layers are available.");
			//TODO: Is it necessary to check if the required instance extensions are available?
		#endif
		}

#ifdef _ENABLE_DEBUG_UTILS
		bool __checkInstanceLayersSupport()
		{
			auto InstanceLayerPropertiesSet = vk::enumerateInstanceLayerProperties();

			std::set<std::string> RequiredInstanceLayerSet(m_InstanceLayerSet.begin(), m_InstanceLayerSet.end());
			for (const auto& LayerProperty : InstanceLayerPropertiesSet) RequiredInstanceLayerSet.erase(LayerProperty.layerName);

			return RequiredInstanceLayerSet.empty();
		}
#endif
	};
}