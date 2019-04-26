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
		if (!_initV()) _THROW_RUNTIME_ERROR("Failed to run application due to failure of initialization!");

		_OUTPUT_EVENT("Succeed to init application.");

		hiveCommon::CCPUTimer CPUTimer;
		while (!m_IsRenderLoopDone)
		{
			CPUTimer.begin();

			if (!_renderV()) _THROW_RUNTIME_ERROR("Render loop interrupted due to render failure!");
			m_IsRenderLoopDone = _isRenderLoopDoneV();

			CPUTimer.end();
			m_FrameInterval = CPUTimer.getElapseTime();
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

	return true;
}

//************************************************************************************
//Function:
bool hiveVKT::CVkApplicationBase::_renderV()
{
	_handleEventV();
	glfwPollEvents();

	m_pCamera->update();

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
	_SAFE_DELETE(m_pCamera);

	m_VkContext.destroyVulkan();

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