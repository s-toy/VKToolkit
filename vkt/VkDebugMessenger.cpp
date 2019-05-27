#include "VkDebugMessenger.h"
#include "VkContext.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
VKAPI_ATTR VkBool32 VKAPI_CALL CVkDebugUtilsMessenger::__debugUtilsCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT vMessageSeverityFlags, 
	vk::DebugUtilsMessageTypeFlagBitsEXT vMessageTypeFlags, const vk::DebugUtilsMessengerCallbackDataEXT* vMessengerCallbackData, void* vUserData)
{
	std::string Severity;

	auto pDebugMessenger = static_cast<CVkDebugUtilsMessenger*>(vUserData);
	_ASSERT(pDebugMessenger);

	if (static_cast<VkDebugUtilsMessageSeverityFlagBitsEXT>(vMessageSeverityFlags) & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) { Severity = "VERBOSE"; }
	else if (static_cast<VkDebugUtilsMessageSeverityFlagBitsEXT>(vMessageSeverityFlags) & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) { Severity = "INFO"; }
	else if (static_cast<VkDebugUtilsMessageSeverityFlagBitsEXT>(vMessageSeverityFlags) & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) 
	{ 
		Severity = "WARNING"; 
#ifdef UNIT_TEST
		pDebugMessenger->m_WarningCount++;
#endif // UNIT_TEST
	}
	else if (static_cast<VkDebugUtilsMessageSeverityFlagBitsEXT>(vMessageSeverityFlags) & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) 
	{ 
		Severity = "ERROR"; 
#ifdef UNIT_TEST
		pDebugMessenger->m_ErrorCount++;
#endif // UNIT_TEST
	}

	if (static_cast<VkDebugUtilsMessageSeverityFlagBitsEXT>(vMessageSeverityFlags) >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		_OUTPUT_WARNING(format("[%s] Validation layer: %s", Severity.c_str(), vMessengerCallbackData->pMessage));

	return VK_FALSE;
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkDebugUtilsMessenger::setupDebugUtilsMessenger()
{
	if (m_pDebugUtilsMessenger)return;

	vk::DebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo;
	DebugUtilsMessengerCreateInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
	DebugUtilsMessengerCreateInfo.pNext = nullptr;
	DebugUtilsMessengerCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
	DebugUtilsMessengerCreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	DebugUtilsMessengerCreateInfo.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(__debugUtilsCallback);
	DebugUtilsMessengerCreateInfo.pUserData = this;

	_ASSERT(CVkContext::getInstance()->isContextCreated());
	auto VulkanInstance = CVkContext::getInstance()->getVulkanInstance();
	auto DynamicDispatchLoader = CVkContext::getInstance()->getDynamicDispatchLoader();

	m_pDebugUtilsMessenger = VulkanInstance.createDebugUtilsMessengerEXT(DebugUtilsMessengerCreateInfo, nullptr, DynamicDispatchLoader);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkDebugUtilsMessenger::destroyDebugUtilsMessenger()
{
	if (m_pDebugUtilsMessenger)
	{
		auto VulkanInstance = CVkContext::getInstance()->getVulkanInstance();
		auto DynamicDispatchLoader = CVkContext::getInstance()->getDynamicDispatchLoader();

		VulkanInstance.destroyDebugUtilsMessengerEXT(m_pDebugUtilsMessenger, nullptr, DynamicDispatchLoader);
		m_pDebugUtilsMessenger = nullptr;
	}
}