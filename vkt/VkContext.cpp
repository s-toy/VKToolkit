#include "VkContext.h"

using namespace hiveVKT;

hiveVKT::CVkContext::CVkContext()
{
}

hiveVKT::CVkContext::~CVkContext()
{
}

//*****************************************************************************************
//FUNCTION:
void CVkContext::createContext()
{
	__createVulkanInstance();
	__createVulkanDevice();

	m_IsInitialized = true;
}

//*****************************************************************************************
//FUNCTION:
void CVkContext::destroyContext()
{
	if (!m_IsInitialized)return;

	m_pDevice.waitIdle();

	m_pDevice.destroyCommandPool(std::get<2>(m_ComprehensiveQueue));
	m_pDevice.destroy();
	m_pInstance.destroy();

	m_PreferDiscreteGpuHint = false;
	m_ForceGraphicsFunctionalityHint = false;
	m_ForceComputeFunctionalityHint = false;
	m_ForceTransferFunctionalityHint = false;
	m_EnableDebugUtilsHint = false;
	m_EnablePresentationHint = false;
	m_EnableApiDumpHint = false;
	m_EnableFpsMonitorHint = false;
	m_EnableScreenshotHint = false;

	m_EnabledInstanceLayers = {};
	m_EnabledInstanceExtensions = {};
	m_EnabledDeviceExtensions = {};
	m_EnabledPhysicalDeviceFeatures = {};

	m_pInstance = nullptr;
	m_pPhysicalDevice = nullptr;
	m_pDevice = nullptr;
	m_ComprehensiveQueue = { UINT32_MAX,nullptr,nullptr };

	m_IsInitialized = false;
}

//*****************************************************************************************
//FUNCTION:
void CVkContext::__createVulkanInstance()
{
	_ASSERT(!m_IsInitialized);

	std::vector<const char*> EnabledInstanceLayers, EnabledInstanceExtensions;
	if (m_EnableDebugUtilsHint)
	{
		EnabledInstanceLayers.emplace_back("VK_LAYER_LUNARG_standard_validation");
		EnabledInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	if (m_EnablePresentationHint)
	{
		EnabledInstanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		EnabledInstanceExtensions.emplace_back("VK_KHR_win32_surface");
	}
	if (m_EnableApiDumpHint)
		EnabledInstanceLayers.emplace_back("VK_LAYER_LUNARG_api_dump");
	if (m_EnableFpsMonitorHint)
		EnabledInstanceLayers.emplace_back("VK_LAYER_LUNARG_monitor");
	if (m_EnableScreenshotHint)
		EnabledInstanceLayers.emplace_back("VK_LAYER_LUNARG_screenshot");
	//TODO：检查是否支持这些层和拓展

	m_EnabledInstanceLayers = std::vector<std::string>(EnabledInstanceLayers.begin(), EnabledInstanceLayers.end());
	m_EnabledInstanceExtensions = std::vector<std::string>(EnabledInstanceExtensions.begin(), EnabledInstanceExtensions.end());

	vk::ApplicationInfo ApplicationInfo = {
		m_ApplicationName.c_str(),m_ApplicationVersion,
		m_EngineName.c_str(),m_EngineVersion,
		m_ApiVersion
	};

	vk::InstanceCreateInfo InstanceCreateInfo = {
		vk::InstanceCreateFlags(),&ApplicationInfo,
		static_cast<uint32_t>(EnabledInstanceLayers.size()),EnabledInstanceLayers.data(),
		static_cast<uint32_t>(EnabledInstanceExtensions.size()),EnabledInstanceExtensions.data()
	};

	m_pInstance = vk::createInstance(InstanceCreateInfo);

	m_DynamicDispatchLoader.init(m_pInstance, nullptr);
}

//*****************************************************************************************
//FUNCTION:
void CVkContext::__createVulkanDevice()
{
	_ASSERT(!m_IsInitialized);

	if (m_EnablePresentationHint)
	{
		_ASSERT(m_pPhysicalDevice.getWin32PresentationSupportKHR(std::get<0>(m_ComprehensiveQueue), m_DynamicDispatchLoader));
		m_EnabledDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}
	//TODO：检查设备是否支持拓展以及相应的物理设备特性

	__pickPhysicalDevice();

	std::vector<const char*> EnabledDeviceExtensions;
	for (auto& DeviceExtension : m_EnabledDeviceExtensions)
		EnabledDeviceExtensions.emplace_back(DeviceExtension.c_str());

	float QueuePriority = 1.0f;
	vk::DeviceQueueCreateInfo DeviceQueueCreateInfo = { vk::DeviceQueueCreateFlags(), std::get<0>(m_ComprehensiveQueue), 1, &QueuePriority };

	vk::DeviceCreateInfo DeviceCreateInfo = {
		vk::DeviceCreateFlags(),
		1,&DeviceQueueCreateInfo,
		0,nullptr,
		static_cast<uint32_t>(EnabledDeviceExtensions.size()),EnabledDeviceExtensions.data(),
		&m_EnabledPhysicalDeviceFeatures
	};

	m_pDevice = m_pPhysicalDevice.createDevice(DeviceCreateInfo);

	std::get<1>(m_ComprehensiveQueue) = m_pDevice.getQueue(std::get<0>(m_ComprehensiveQueue), 0);
	std::get<2>(m_ComprehensiveQueue) = m_pDevice.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, std::get<0>(m_ComprehensiveQueue)));
}

//*****************************************************************************************
//FUNCTION:
void hiveVKT::CVkContext::__pickPhysicalDevice()
{
	auto PhysicalDevices = m_pInstance.enumeratePhysicalDevices();
	_ASSERT_EXPR(!PhysicalDevices.empty(), "No physical device that support Vulkan");

	auto PickPhysicalDevice = [&](bool vPreferDiscreteGpu) {
		for (auto PhysicalDevice : PhysicalDevices)
		{
			if (vPreferDiscreteGpu)
			{
				auto PhysicalDeviceProperties = PhysicalDevice.getProperties();
				if (PhysicalDeviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
					break;
			}

			auto QueueFamilyProperties = PhysicalDevice.getQueueFamilyProperties();
			_ASSERT(!QueueFamilyProperties.empty());
			uint32_t ComprehensiveQueueFlags = 0;
			uint32_t ComprehensiveQueueFamilyIndex = UINT32_MAX;

			for (auto i = 0; i < QueueFamilyProperties.size(); ++i)
			{
				if (static_cast<uint32_t>(QueueFamilyProperties[i].queueFlags) > ComprehensiveQueueFlags)
				{
					if (m_ForceGraphicsFunctionalityHint)
						if (!(QueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics))
							break;

					if (m_ForceComputeFunctionalityHint)
						if (!(QueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute))
							break;

					if (m_ForceTransferFunctionalityHint)
						if (!(QueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer))
							break;

					ComprehensiveQueueFlags = static_cast<uint32_t>(QueueFamilyProperties[i].queueFlags);
					ComprehensiveQueueFamilyIndex = i;
				}
			}

			if (ComprehensiveQueueFamilyIndex != UINT32_MAX)
			{
				m_ComprehensiveQueue = std::tuple(ComprehensiveQueueFamilyIndex, nullptr, nullptr);
				m_pPhysicalDevice = PhysicalDevice;
				return;
			}
		}
	};

	PickPhysicalDevice(m_PreferDiscreteGpuHint);

	if (m_PreferDiscreteGpuHint && (!m_pPhysicalDevice || std::get<0>(m_ComprehensiveQueue) == UINT32_MAX))
		PickPhysicalDevice(false);

	_ASSERT(m_pPhysicalDevice && std::get<0>(m_ComprehensiveQueue) != UINT32_MAX);
}
