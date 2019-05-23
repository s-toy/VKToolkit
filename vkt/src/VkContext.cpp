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
bool CVkContext::initVulkan(const std::vector<const char*>& vExtensions4Instance, const std::vector<const char*>& vLayers4Instance, const std::vector<const char*>& vExtensions4Device, const std::vector<const char*>& vLayers4Device, GLFWwindow* vWindow, const vk::PhysicalDeviceFeatures& vEnabledFeatures)
{
	__checkExtensions(vExtensions4Instance, vExtensions4Device);

	__createInstance(vExtensions4Instance, vLayers4Instance);
	__createDebugMessenger();
	if (m_EnabledPresentation) __createSurface(vWindow);
	__pickPhysicalDevice();
	__findRequiredQueueFamilies(m_VkPhysicalDevice);
	__createDevice(vExtensions4Device, vLayers4Device, vEnabledFeatures);
	__createCommandPool();

	if (m_EnabledPresentation)
	{
		int Width, Height;
		glfwGetFramebufferSize(vWindow, &Width, &Height);
		__createSwapChain(Width, Height);
		__createImageViews();
	}

	return true;
}

//************************************************************************************
//Function:
void CVkContext::__createInstance(const std::vector<const char*>& vExtensions4Instance, const std::vector<const char*>& vLayers4Instance)
{
	hiveVKT::CVkInstanceCreator InstanceCreator;
	InstanceCreator.setEnabledExtensions(vExtensions4Instance);
	InstanceCreator.setEnabledLayers(vLayers4Instance);
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
void CVkContext::__createDevice(const std::vector<const char*>& vExtensions4Device, const std::vector<const char*>& vLayers4Device, const vk::PhysicalDeviceFeatures& vEnabledFeatures)
{
	hiveVKT::CVkDeviceCreator DeviceCreator;
	DeviceCreator.setEnabledExtensions(vExtensions4Device);
	DeviceCreator.setEnabledLayers(vLayers4Device);
	DeviceCreator.addQueue(m_RequiredQueueFamilyIndices.QueueFamily.value(), 1, 1.0f);
	DeviceCreator.setPhysicalDeviceFeatures(&vEnabledFeatures);
	m_VkDevice = DeviceCreator.create(m_VkPhysicalDevice);

	m_VkQueue = m_VkDevice.getQueue(m_RequiredQueueFamilyIndices.QueueFamily.value(), 0);
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
	m_SwapChainImages = m_VkDevice.getSwapchainImagesKHR(m_VkSwapchain);
}

//************************************************************************************
//Function:
void CVkContext::__createImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());
	for (size_t i = 0; i < m_SwapChainImages.size(); ++i)
	{
		vk::ImageViewCreateInfo CreateInfo = {};
		CreateInfo.image = m_SwapChainImages[i];
		CreateInfo.viewType = vk::ImageViewType::e2D;
		CreateInfo.format = m_SwapChainImageFormat;
		CreateInfo.components = vk::ComponentSwizzle::eIdentity;
		CreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor,0,1,0,1 };
		m_SwapChainImageViews[i] = m_VkDevice.createImageView(CreateInfo);
	}
}

//************************************************************************************
//Function:
void CVkContext::__checkExtensions(const std::vector<const char*>& vExtensions4Instance, const std::vector<const char*>& vExtensions4Device)
{
	std::vector<std::string> RequiredExtensionSet(vExtensions4Instance.begin(), vExtensions4Instance.end());
	RequiredExtensionSet.insert(RequiredExtensionSet.end(), vExtensions4Device.begin(), vExtensions4Device.end());

	if (std::find(RequiredExtensionSet.begin(), RequiredExtensionSet.end(), VK_KHR_SURFACE_EXTENSION_NAME) == RequiredExtensionSet.end() ||
		std::find(RequiredExtensionSet.begin(), RequiredExtensionSet.end(), "VK_KHR_win32_surface") == RequiredExtensionSet.end() ||
		std::find(RequiredExtensionSet.begin(), RequiredExtensionSet.end(), VK_KHR_SWAPCHAIN_EXTENSION_NAME) == RequiredExtensionSet.end())
	{
		m_EnabledPresentation = false;
	}
	else
	{
		m_EnabledPresentation = true;
	}
}

//************************************************************************************
//Function:
void hiveVKT::CVkContext::__createCommandPool()
{
	vk::CommandPoolCreateInfo CommandPoolCreateInfo;
	CommandPoolCreateInfo.flags = vk::CommandPoolCreateFlags();
	CommandPoolCreateInfo.queueFamilyIndex = m_RequiredQueueFamilyIndices.QueueFamily.value();

	m_VkCommandPool = m_VkDevice.createCommandPool(CommandPoolCreateInfo);
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
		
		if (GraphicsSupport && TransferSupport)
		{
			if (m_EnabledPresentation)
			{
				PresentSupport = static_cast<VkBool32>(vPhysicalDevice.getSurfaceSupportKHR(i, m_VkSurface));
				if (PresentSupport)
				{
					m_RequiredQueueFamilyIndices.QueueFamily = i;
					break;
				}
			}
			else
			{
				m_RequiredQueueFamilyIndices.QueueFamily = i;
				break;
			}
		}
		i++;
	}

	return m_RequiredQueueFamilyIndices; //HACK:
}

//************************************************************************************
//Function:
void hiveVKT::CVkContext::destroyVulkan()
{
	m_VkDevice.destroyCommandPool(m_VkCommandPool);

	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		m_VkDevice.destroyImageView(m_SwapChainImageViews[i]);
	}
	if (m_EnabledPresentation) m_VkDevice.destroySwapchainKHR(m_VkSwapchain);
	m_VkDevice.destroy();

	m_pDebugMessenger->destroyDebugMessenger(m_VkInstance);
	_SAFE_DELETE(m_pDebugMessenger);

	if (m_EnabledPresentation) m_VkInstance.destroySurfaceKHR(m_VkSurface);
	m_VkInstance.destroy();
}