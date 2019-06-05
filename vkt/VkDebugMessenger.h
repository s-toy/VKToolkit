#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"
#include "Common.h"
#include "Export.h"

namespace hiveVKT
{
	class VKT_DECLSPEC CVkDebugUtilsMessenger
	{
	public:
		vk::Result setupDebugUtilsMessenger(const vk::Instance& vInstance,const vk::DispatchLoaderDynamic& vDynamicDispatchLoader);
		void destroyDebugUtilsMessenger(const vk::Instance& vInstance, const vk::DispatchLoaderDynamic& vDynamicDispatchLoader);

		uint32_t getWarningAndErrorCount() const { return m_WarningCount + m_ErrorCount; }
		uint32_t getWarningCount() const { return m_WarningCount; }
		uint32_t getErrorCount() const { return m_ErrorCount; }

	private:
		vk::DebugUtilsMessengerEXT m_pDebugUtilsMessenger = nullptr;
		uint32_t m_WarningCount = 0, m_ErrorCount = 0;

		static VKAPI_ATTR VkBool32 VKAPI_CALL __debugUtilsCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, 
			vk::DebugUtilsMessageTypeFlagBitsEXT vMessageTypeFlags, const vk::DebugUtilsMessengerCallbackDataEXT* vMessengerCallbackData, void* vUserData);
	};
}