#include "VkContext.hpp"
#include "VkInstanceCreator.hpp"
#include "VkDeviceCreator.hpp"
#include "VkDebugMessenger.hpp"
#include "VkPhysicalDeviceInfoHelper.hpp"

using namespace hiveVKT;

CVkContext::CVkContext()
{

}

CVkContext::~CVkContext()
{

}

//************************************************************************************
//Function:
bool CVkContext::initVulkan(GLFWwindow* vWindow, const std::vector<const char *>& vExtensions /* = "VK_KHR_swapchain" */)
{
	__createInstance();
	__createDebugMessenger();
	__createSurface(vWindow);
	__pickPhysicalDevice();
	__findRequiredQueueFamilies(m_VkPhysicalDevice);
	__createDevice();

	int Width, Height;
	glfwGetFramebufferSize(vWindow, &Width, &Height);
	__createSwapChain(Width, Height);

	return true;
}

//************************************************************************************
//Function:
void CVkContext::__createInstance()
{
	hiveVKT::CVkInstanceCreator InstanceCreator;
	m_VkInstance = InstanceCreator.create();
}

//************************************************************************************
//Function:
void CVkContext::__createDebugMessenger()
{
	m_pDebugMessenger = new CVkDebugMessenger;
	m_pDebugMessenger->setupDebugMessenger(m_VkInstance);
}

//************************************************************************************
//Function:
void CVkContext::__createSurface(GLFWwindow* vWindow)
{
	if (glfwCreateWindowSurface(m_VkInstance, vWindow, nullptr, &m_VkSurface) != VK_SUCCESS)
		_THROW_RUNTIME_ERROR("Failed to create window surface!");
}

//************************************************************************************
//Function:
void CVkContext::__pickPhysicalDevice()
{
	auto PhysicalDeviceSet = m_VkInstance.enumeratePhysicalDevices();
	_ASSERTE(!PhysicalDeviceSet.empty());
	m_VkPhysicalDevice = PhysicalDeviceSet[0];	//TODO: check whether the physical device is suitable.

	CVkPhysicalDeviceInfoHelper::getInstance()->init(m_VkPhysicalDevice);
}

//************************************************************************************
//Function:
void CVkContext::__createDevice()
{
	hiveVKT::CVkDeviceCreator DeviceCreator;
	DeviceCreator.addQueue(m_RequiredQueueFamilyIndices.QueueFamily.value(), 1, 1.0f);
	DeviceCreator.setPhysicalDeviceFeatures(&m_VkPhysicalDeviceFeatures);
	m_VkDevice = DeviceCreator.create(m_VkPhysicalDevice);
}

//************************************************************************************
//Function:
void CVkContext::__createSwapChain(int vWidth, int vHeight)
{
	CVkSwapChainCreator SwapchainCreator;
	m_VkSwapchain = SwapchainCreator.create(m_VkSurface, m_VkDevice, m_VkPhysicalDevice, vWidth, vHeight);

	m_SwapChainSupportDetails = SwapchainCreator.queryPhysicalDeviceSwapChainSupport(m_VkSurface, m_VkPhysicalDevice);
	m_SwapChainImageFormat = SwapchainCreator.getSwapChainImageFormat();
	m_SwapChainExtent = SwapchainCreator.getSwapChainExtent();
}

//************************************************************************************
//Function:
SQueueFamilyIndices CVkContext::__findRequiredQueueFamilies(const vk::PhysicalDevice& vPhysicalDevice)
{
	auto QueueFamilyPropertySet = vPhysicalDevice.getQueueFamilyProperties();

	int i = 0;
	for (const auto& QueueFamilyProperty : QueueFamilyPropertySet)
	{
		VkBool32 GraphicsSupport = VK_FALSE, PresentSupport = VK_FALSE, TransferSupport = VK_FALSE;

		GraphicsSupport = static_cast<VkBool32>(QueueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics);
		TransferSupport = static_cast<VkBool32>(QueueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer);
		PresentSupport = static_cast<VkBool32>(vPhysicalDevice.getSurfaceSupportKHR(i, m_VkSurface));

		if (GraphicsSupport && PresentSupport && TransferSupport)
		{
			m_RequiredQueueFamilyIndices.QueueFamily = i;
			break;
		}

		i++;
	}

	return m_RequiredQueueFamilyIndices; //HACK:
}

//************************************************************************************
//Function:
void hiveVKT::CVkContext::destroyVulkan()
{
	m_VkDevice.destroySwapchainKHR(m_VkSwapchain);
	m_VkDevice.destroy();

	m_pDebugMessenger->destroyDebugMessenger(m_VkInstance);
	_SAFE_DELETE(m_pDebugMessenger);

	m_VkInstance.destroySurfaceKHR(m_VkSurface);
	m_VkInstance.destroy();
}