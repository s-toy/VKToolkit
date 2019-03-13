#pragma once
#include <iostream>
#include <vulkan/vulkan.hpp>
#include "Utility.hpp"
#include "Common.hpp"

namespace hiveVKT
{
	class CVkDebugMessenger
	{
	public:
		bool setupDebugMessenger(const vk::Instance& vInstance)
		{
		#ifdef _ENABLE_VK_DEBUG_UTILS
			if (m_pDebugUtilsMessenger) return;

			VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo = {};
			DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			DebugUtilsMessengerCreateInfo.pfnUserCallback = __debugCallback;
			DebugUtilsMessengerCreateInfo.pUserData = nullptr;

			if (__createDebugUtilsMessengerEXT(vInstance, &DebugUtilsMessengerCreateInfo, nullptr, &m_pDebugUtilsMessenger) != VK_SUCCESS)
			{
				_OUTPUT_WARNING("Failed to set up debug utils messenger!");
				return false;
			}

			return true;
		#else
			return false;
		#endif
		}

		void destroyDebugMessenger(const vk::Instance& vInstance)
		{
		#ifdef _ENABLE_VK_DEBUG_UTILS
			if (!m_pDebugUtilsMessenger) return;
			__destroyDebugUtilsMessengerEXT(vInstance, m_pDebugUtilsMessenger, nullptr);
		#endif
		}

	private:
		VkDebugUtilsMessengerEXT m_pDebugUtilsMessenger = VK_NULL_HANDLE;

#ifdef _ENABLE_VK_DEBUG_UTILS
		static VKAPI_ATTR VkBool32 VKAPI_CALL __debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, VkDebugUtilsMessageTypeFlagsEXT vMessageTypeFlags, const VkDebugUtilsMessengerCallbackDataEXT* vCallbackData, void* vUserData)
		{
			std::string Prefix;

			if (vMessageSeverityFlags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) { Prefix = "VERBOSE"; }
			else if (vMessageSeverityFlags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) { Prefix = "INFO"; }
			else if (vMessageSeverityFlags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) { Prefix = "WARNING"; }
			else if (vMessageSeverityFlags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) { Prefix = "ERROR"; }
			else if (vMessageSeverityFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) { Prefix = "DEBUG"; }

			if (vMessageSeverityFlags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
				_OUTPUT_WARNING(format("[%s] Validation layer: %s", Prefix.c_str(), vCallbackData->pMessage));

			return VK_FALSE;
		}

		static VkResult __createDebugUtilsMessengerEXT(const vk::Instance& vInstance, const VkDebugUtilsMessengerCreateInfoEXT* vDebugUtilsMessengerCreateInfo, const VkAllocationCallbacks* vAllocator, VkDebugUtilsMessengerEXT* voDebugUtilsMessenger)
		{
			auto Function = (PFN_vkCreateDebugUtilsMessengerEXT)vInstance.getProcAddr("vkCreateDebugUtilsMessengerEXT");

			if (Function != nullptr)
				return Function(vInstance, vDebugUtilsMessengerCreateInfo, vAllocator, voDebugUtilsMessenger);
			else
				return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		static void __destroyDebugUtilsMessengerEXT(const vk::Instance& vInstance, VkDebugUtilsMessengerEXT vDebugUtilsMessenger, const VkAllocationCallbacks* pAllocator)
		{
			auto Function = (PFN_vkDestroyDebugUtilsMessengerEXT)vInstance.getProcAddr("vkDestroyDebugUtilsMessengerEXT");

			_ASSERT(Function != nullptr);
			Function(vInstance, vDebugUtilsMessenger, pAllocator);
		}
#endif
	};
}