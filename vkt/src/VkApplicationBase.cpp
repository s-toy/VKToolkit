#include "VkApplicationBase.hpp"
#include "VkInstanceCreator.hpp"
#include "VkDeviceCreator.hpp"
#include "VkDebugMessenger.hpp"
#include "WindowCreator.hpp"
#include "Utility.hpp"

using namespace hiveVKT;

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::run()
{
	try
	{
		if (!_initV()) _THROW_RUNTINE_ERROR("Failed to run application due to failure of initialization!");

		_OUTPUT_EVENT("Succeed to init application.");

		while (!m_IsRenderLoopDone)
		{
			if (!_renderV()) _THROW_RUNTINE_ERROR("Render loop interrupted due to render failure!");
			m_IsRenderLoopDone = _isRenderLoopDoneV();
		}

		_destroyV();

		_OUTPUT_EVENT("Succeed to end application.");
	}
	catch (const std::runtime_error& e)
	{
		_OUTPUT_WARNING(e.what()); exit(EXIT_FAILURE);			//TODO: how to handle exceptions
	}
	catch (...)
	{
		_OUTPUT_WARNING("The program is terminated due to unexpected error!"); exit(EXIT_FAILURE);
	}
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::_initV()
{
	if (!__initWindow()) { _OUTPUT_WARNING("Failed to initialize application due to failure of initializing window!"); return false; }
	if (!__initVulkan()) { _OUTPUT_WARNING("Failed to initialize application due to failure of initializing vulkan!"); return false; }

	return true;
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::_renderV()
{
	_handleEventV();
	glfwPollEvents();

	return true;
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::_isRenderLoopDoneV()
{
	_ASSERTE(m_pWindow);

	bool IsRenderLoopDone = glfwWindowShouldClose(m_pWindow);
	if (IsRenderLoopDone) { glfwDestroyWindow(m_pWindow); glfwTerminate(); }

	return IsRenderLoopDone;
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::_destroyV()
{
	m_pDebugMessenger->destroyDebugMessenger(m_VkInstance);
	_SAFE_DELETE(m_pDebugMessenger);

	m_VkInstance.destroySurfaceKHR(m_VkSurface);
	m_VkInstance.destroy();

	glfwTerminate();
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::__initWindow()
{
	_ASSERTE(!m_pWindow);

	CWindowCreator WindowCreator;
	m_pWindow = WindowCreator.create(m_DisplayInfo);

	if (!m_pWindow) return false;

	return true;
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::__initVulkan()
{
	__prepareLayersAndExtensions();
	__createInstance();
	__createDebugMessenger();
	__createSurface();
	__pickPhysicalDevice();
	__createDevice();

	return true;
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__prepareLayersAndExtensions()
{
#ifdef _ENABLE_DEBUG_UTILS
	m_EnabledLayersAtDeviceLevel.emplace_back("VK_LAYER_LUNARG_standard_validation");
#endif

	m_EnabledExtensionsAtDeviceLevel.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__createInstance()
{
	hiveVKT::CVkInstanceCreator InstanceCreator;
	m_VkInstance = InstanceCreator.create();
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__createDebugMessenger()
{
	m_pDebugMessenger = new CVkDebugMessenger;
	m_pDebugMessenger->setupDebugMessenger(m_VkInstance);
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__createSurface()
{
	if (glfwCreateWindowSurface(m_VkInstance, _window(), nullptr, &m_VkSurface) != VK_SUCCESS)
		_THROW_RUNTINE_ERROR("Failed to create window surface!");
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__pickPhysicalDevice()
{
	auto PhysicalDeviceSet = m_VkInstance.enumeratePhysicalDevices();

	bool IsDeviceFound = false;
	for (auto PhysicalDevice : PhysicalDeviceSet)
	{
		if (__isPhysicalDeviceSuitable(PhysicalDevice))
		{
			m_VkPhysicalDevice = PhysicalDevice;
			IsDeviceFound = true;
			break;
		}
	}

	if (!IsDeviceFound)	_THROW_RUNTINE_ERROR("Failed to find a suitable GPU!");
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__createDevice()
{
	hiveVKT::CVkDeviceCreator DeviceCreator;

	DeviceCreator.addQueue(m_RequiredQueueFamilyIndices.QueueFamily.value(), 1, 1.0f);

	vk::PhysicalDeviceFeatures PhysicalDeviceFeatures;
	PhysicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	PhysicalDeviceFeatures.sampleRateShading = VK_TRUE;
	DeviceCreator.setPhysicalDeviceFeatures(&PhysicalDeviceFeatures);

	m_VkDevice = DeviceCreator.create(_physicalDevice());
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::__isPhysicalDeviceSuitable(const vk::PhysicalDevice& vPhysicalDevice)
{
	SQueueFamilyIndices QueueFamily = __findRequiredQueueFamilies(vPhysicalDevice);
	bool IsExtensionSupport = __checkPhysicalDeviceExtensionSupport(vPhysicalDevice);
	bool IsSwapChainAdequate = false;
	if (IsExtensionSupport)
	{
		SSwapChainSupportDetails SwapChainSupportDetails = __queryPhysicalDeviceSwapChainSupport(vPhysicalDevice);
		IsSwapChainAdequate = !SwapChainSupportDetails.SurfaceFormatSet.empty() && !SwapChainSupportDetails.PresentModeSet.empty();
	}

	return QueueFamily.IsComplete() && IsExtensionSupport && IsSwapChainAdequate;
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::__checkPhysicalDeviceExtensionSupport(const vk::PhysicalDevice& vPhysicalDevice) const
{
	auto PhysicalDeviceExtensionPropertySet = vPhysicalDevice.enumerateDeviceExtensionProperties();

	std::set<std::string> RequiredPhysicalDeviceExtensionSet(m_EnabledExtensionsAtDeviceLevel.begin(), m_EnabledExtensionsAtDeviceLevel.end());

	for (const auto& ExtensionProperty : PhysicalDeviceExtensionPropertySet)
		RequiredPhysicalDeviceExtensionSet.erase(ExtensionProperty.extensionName);

	return RequiredPhysicalDeviceExtensionSet.empty();
}

//************************************************************************************
//Function:
SQueueFamilyIndices hiveVKT::CVkApplicationBase::__findRequiredQueueFamilies(const vk::PhysicalDevice& vPhysicalDevice)
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
		}
	}

	return m_RequiredQueueFamilyIndices;
}

//************************************************************************************
//Function:
SSwapChainSupportDetails hiveVKT::CVkApplicationBase::__queryPhysicalDeviceSwapChainSupport(const vk::PhysicalDevice& vPhysicalDevice)
{
	_ASSERTE(m_VkSurface != VK_NULL_HANDLE);

	m_SwapChainSupportDetails.SurfaceCapabilities = vPhysicalDevice.getSurfaceCapabilitiesKHR(m_VkSurface);
	m_SwapChainSupportDetails.SurfaceFormatSet = vPhysicalDevice.getSurfaceFormatsKHR(m_VkSurface);
	m_SwapChainSupportDetails.PresentModeSet = vPhysicalDevice.getSurfacePresentModesKHR(m_VkSurface);

	return m_SwapChainSupportDetails;
}