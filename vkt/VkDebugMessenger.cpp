#include "VkDebugMessenger.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
#ifdef _ENABLE_DEBUG_UTILS
VKAPI_ATTR VkBool32 VKAPI_CALL CVkDebugMessenger::__debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, 
	VkDebugUtilsMessageTypeFlagsEXT vMessageTypeFlags, const VkDebugUtilsMessengerCallbackDataEXT* vCallbackData, void* vUserData)
{
	std::string Severity;

	auto pDebugMessenger = static_cast<CVkDebugMessenger*>(vUserData);
	_ASSERT(pDebugMessenger);

	if (vMessageSeverityFlags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) { Severity = "VERBOSE"; }
	else if (vMessageSeverityFlags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) { Severity = "INFO"; }
	else if (vMessageSeverityFlags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) { Severity = "WARNING"; pDebugMessenger->m_WarningCount++; }
	else if (vMessageSeverityFlags & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) { Severity = "ERROR"; pDebugMessenger->m_ErrorCount++; }

	if (vMessageSeverityFlags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		_OUTPUT_WARNING(format("[%s] Validation layer: %s", Severity.c_str(), vCallbackData->pMessage));

	return VK_FALSE;
}

//***********************************************************************************************
//FUNCTION:
VkResult __createDebugUtilsMessengerEXT(const vk::Instance& vInstance, const VkDebugUtilsMessengerCreateInfoEXT* vDebugUtilsMessengerCreateInfo, const VkAllocationCallbacks* vAllocator, VkDebugUtilsMessengerEXT* voDebugUtilsMessenger)
{
	auto Function = (PFN_vkCreateDebugUtilsMessengerEXT)vInstance.getProcAddr("vkCreateDebugUtilsMessengerEXT");

	if (Function != nullptr)
		return Function(vInstance, vDebugUtilsMessengerCreateInfo, vAllocator, voDebugUtilsMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

//***********************************************************************************************
//FUNCTION:
void __destroyDebugUtilsMessengerEXT(const vk::Instance& vInstance, VkDebugUtilsMessengerEXT vDebugUtilsMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto Function = (PFN_vkDestroyDebugUtilsMessengerEXT)vInstance.getProcAddr("vkDestroyDebugUtilsMessengerEXT");

	_ASSERT(Function != nullptr);
	Function(vInstance, vDebugUtilsMessenger, pAllocator);
}
#endif

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::CVkDebugMessenger::setupDebugMessenger(const vk::Instance& vInstance)
{
#ifdef _ENABLE_DEBUG_UTILS
	if (m_pDebugUtilsMessenger) return true;

	VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo = {};
	DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	DebugUtilsMessengerCreateInfo.pfnUserCallback = __debugCallback;
	DebugUtilsMessengerCreateInfo.pUserData = this;

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

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkDebugMessenger::destroyDebugMessenger(const vk::Instance& vInstance)
{
#ifdef _ENABLE_DEBUG_UTILS
	if (!m_pDebugUtilsMessenger) return;
	__destroyDebugUtilsMessengerEXT(vInstance, m_pDebugUtilsMessenger, nullptr);
#endif
}