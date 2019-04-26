#include "VkApplicationBase.hpp"
#include "common/CpuTimer.h"
#include "WindowCreator.hpp"
#include "Utility.hpp"
#include "InputManager.hpp"
#include "Camera.hpp"

using namespace hiveVKT;

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::run()
{
	try
	{
		__awake();
		if (!__init()) _THROW_RUNTIME_ERROR("Failed to run application due to failure of initialization!");

		_OUTPUT_EVENT("Succeed to init application.");

		hiveCommon::CCPUTimer CPUTimer;
		while (!glfwWindowShouldClose(m_pWindow))
		{
			CPUTimer.begin();

			__udpate();

			CPUTimer.end();
			m_FrameInterval = CPUTimer.getElapseTime();
		}

		__destroy();

		_OUTPUT_EVENT("Succeed to end application.");
	}
	catch (const std::runtime_error& e)
	{
		_OUTPUT_WARNING(e.what());
		exit(EXIT_FAILURE);
	}
	catch (...)
	{
		_OUTPUT_WARNING("The program is terminated due to unexpected error!"); exit(EXIT_FAILURE);
	}
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
void hiveVKT::CVkApplicationBase::__awake()
{
	_awakeV();
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::__init()
{
	if (!__initWindow()) { _OUTPUT_WARNING("Failed to initialize application due to failure of initializing window!"); return false; }

	std::vector<const char*> InstanceLayers = {};
	std::vector<const char*> InstanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME,"VK_KHR_win32_surface" };//这些应该是通过glfwGetRequiredInstanceExtensions获取的
	std::vector<const char*> DeviceLayers = {};
	std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef _ENABLE_DEBUG_UTILS
	InstanceLayers.emplace_back("VK_LAYER_LUNARG_standard_validation");
	InstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	DeviceLayers.emplace_back("VK_LAYER_LUNARG_standard_validation");
#endif

	if (!m_VkContext.initVulkan(InstanceExtensions, InstanceLayers, DeviceExtensions, DeviceLayers, m_pWindow, m_EnabledPhysicalDeviceFeatures)) { _OUTPUT_WARNING("Failed to initialize application due to failure of initializing vulkan!"); return false; }

	CInputManager::getInstance()->init(m_pWindow);
	m_pCamera = new CCamera(glm::vec3(0.0f, 0.0f, 7.0f), (double)m_WindowCreateInfo.WindowWidth / m_WindowCreateInfo.WindowHeight);

	return _initV();
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__udpate()
{
	glfwPollEvents();
	m_pCamera->update();

	_updateV();
}

//************************************************************************************
//Function:
void hiveVKT::CVkApplicationBase::__destroy()
{
	_destroyV();

	_SAFE_DELETE(m_pCamera);
	m_VkContext.destroyVulkan();

	glfwTerminate();
}