#include "Common.h"
#include <fstream>

using namespace DeferredShading;

//************************************************************************************
//Function:
VkResult DeferredShading::CreateDebugUtilsMessengerEXT(VkInstance vVulkanInstance, const VkDebugUtilsMessengerCreateInfoEXT* pDebugUtilsMessengerCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugUtilsMessenger)
{
	auto Function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vVulkanInstance, "vkCreateDebugUtilsMessengerEXT");
	if (Function != nullptr)
		return Function(vVulkanInstance, pDebugUtilsMessengerCreateInfo, pAllocator, pDebugUtilsMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

//************************************************************************************
//Function:
void DeferredShading::DestroyDebugUtilsMessengerEXT(VkInstance vVulkanInstance, VkDebugUtilsMessengerEXT vDebugUtilsMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto Function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vVulkanInstance, "vkDestroyDebugUtilsMessengerEXT");

	_ASSERT(Function != nullptr);
	Function(vVulkanInstance, vDebugUtilsMessenger, pAllocator);
}

//************************************************************************************
//Function:
std::vector<char> DeferredShading::ReadFile(const std::string & vFileName)
{
	std::ifstream File(vFileName, std::ios::ate | std::ios::binary);

	if (!File.is_open())
		throw std::runtime_error(("Failed to open file [" + vFileName + "]!").c_str());

	rsize_t FileSize = static_cast<rsize_t>(File.tellg());
	std::vector<char> Buffer(FileSize);

	File.seekg(0);
	File.read(Buffer.data(), FileSize);

	File.close();

	return Buffer;
}
