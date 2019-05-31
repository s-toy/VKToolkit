#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"
#include "Common.h"

namespace hiveVKT
{
	class CVkDebugUtilsMessenger
	{
	public:
		vk::Result setupDebugUtilsMessenger(const vk::Instance& vInstance,const vk::DispatchLoaderDynamic& vDynamicDispatchLoader);
		void destroyDebugUtilsMessenger(const vk::Instance& vInstance, const vk::DispatchLoaderDynamic& vDynamicDispatchLoader);

#ifdef UNIT_TEST
		uint32_t getWarningAndErrorCount() const { return m_WarningCount + m_ErrorCount; }
		uint32_t getWarningCount() const { return m_WarningCount; }
		uint32_t getErrorCount() const { return m_ErrorCount; }
#endif // UNIT_TEST		

	private:
		vk::DebugUtilsMessengerEXT m_pDebugUtilsMessenger = nullptr;

#ifdef UNIT_TEST
		uint32_t m_WarningCount = 0, m_ErrorCount = 0;
#endif // UNIT_TEST

		static VKAPI_ATTR VkBool32 VKAPI_CALL __debugUtilsCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, 
			vk::DebugUtilsMessageTypeFlagBitsEXT vMessageTypeFlags, const vk::DebugUtilsMessengerCallbackDataEXT* vMessengerCallbackData, void* vUserData);
	};
}