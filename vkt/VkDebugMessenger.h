#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"
#include "Common.h"

namespace hiveVKT
{
	class CVkDebugMessenger
	{
	public:
		bool setupDebugMessenger(const vk::Instance& vInstance);
		void destroyDebugMessenger(const vk::Instance& vInstance);

#ifdef _ENABLE_DEBUG_UTILS
		uint32_t getWarningAndErrorCount() const { return m_WarningCount + m_ErrorCount; }
		uint32_t getWarningCount() const { return m_WarningCount; }
		uint32_t getErrorCount() const { return m_ErrorCount; }
#endif

	private:
#ifdef _ENABLE_DEBUG_UTILS
		VkDebugUtilsMessengerEXT m_pDebugUtilsMessenger = VK_NULL_HANDLE;
		uint32_t m_WarningCount = 0, m_ErrorCount = 0;

		static VKAPI_ATTR VkBool32 VKAPI_CALL __debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, 
			VkDebugUtilsMessageTypeFlagsEXT vMessageTypeFlags, const VkDebugUtilsMessengerCallbackDataEXT* vCallbackData, void* vUserData);
#endif
	};
}