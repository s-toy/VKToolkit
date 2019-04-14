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

		hiveCommon::CCpuTimer CPUTimer;
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
	if (!m_VkContext.initVulkan(m_pWindow)) { _OUTPUT_WARNING("Failed to initialize application due to failure of initializing vulkan!"); return false; }

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