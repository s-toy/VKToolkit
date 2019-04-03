#include "VkApplicationBase.hpp"
#include "VkInstanceCreator.hpp"
#include "VkDeviceCreator.hpp"
#include "VkDebugMessenger.hpp"
#include "WindowCreator.hpp"
#include "Utility.hpp"
#include "InputManager.hpp"

using namespace hiveVKT;

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::run()
{
	try
	{
		if (!_initV()) _THROW_RUNTIME_ERROR("Failed to run application due to failure of initialization!");

		_OUTPUT_EVENT("Succeed to init application.");

		while (!m_IsRenderLoopDone)
		{
			if (!_renderV()) _THROW_RUNTIME_ERROR("Render loop interrupted due to render failure!");
			m_IsRenderLoopDone = _isRenderLoopDoneV();
		}

		_destroyV();

		_OUTPUT_EVENT("Succeed to end application.");
	}
	catch (const std::runtime_error& e)
	{
		_OUTPUT_WARNING(e.what());
		exit(EXIT_FAILURE);			//TODO: how to handle exceptions
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

	CInputManager::getInstance()->init(m_pWindow);

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
	m_VkDevice.destroySwapchainKHR(m_VkSwapchain);
	m_VkDevice.destroy();

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
	m_pWindow = WindowCreator.create(m_WindowCreateInfo);

	return m_pWindow ? true : false;
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::__initVulkan()
{
	__createInstance();
	__createDebugMessenger();
	__createSurface();
	__pickPhysicalDevice();
	__createDevice();
	__createSwapChain();

	return true;
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
		_THROW_RUNTIME_ERROR("Failed to create window surface!");
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__pickPhysicalDevice()
{
	auto PhysicalDeviceSet = m_VkInstance.enumeratePhysicalDevices();
	_ASSERTE(!PhysicalDeviceSet.empty());
	m_VkPhysicalDevice = PhysicalDeviceSet[0];	//TODO: check whether the physical device is suitable.

	m_VkPhysicalDeviceMemoryProperties = m_VkPhysicalDevice.getMemoryProperties();

	__findRequiredQueueFamilies(m_VkPhysicalDevice);
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__createDevice()
{
	hiveVKT::CVkDeviceCreator DeviceCreator;
	DeviceCreator.addQueue(m_RequiredQueueFamilyIndices.QueueFamily.value(), 1, 1.0f);
	DeviceCreator.setPhysicalDeviceFeatures(&m_VkPhysicalDeviceFeatures);
	m_VkDevice = DeviceCreator.create(_physicalDevice());
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__createSwapChain()
{
	CVkSwapChainCreator SwapchainCreator;
	m_VkSwapchain = SwapchainCreator.create(m_VkSurface, m_VkDevice, m_VkPhysicalDevice, m_WindowCreateInfo.WindowWidth, m_WindowCreateInfo.WindowHeight);

	m_SwapChainSupportDetails = SwapchainCreator.queryPhysicalDeviceSwapChainSupport(m_VkSurface, m_VkPhysicalDevice);
	m_SwapChainImageFormat = SwapchainCreator.getSwapChainImageFormat();
	m_SwapChainExtent = SwapchainCreator.getSwapChainExtent();
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
			break;
		}

		i++;
	}

	return m_RequiredQueueFamilyIndices; //HACK:
}