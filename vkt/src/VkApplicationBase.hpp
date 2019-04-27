#pragma once
#include <optional>
#include <GLFW/glfw3.h>
#include "Common.hpp"
#include "VkContext.hpp"

namespace hiveVKT
{
	class CCamera;

	class CVkApplicationBase
	{
	public:
		CVkApplicationBase() = default;
		virtual ~CVkApplicationBase() = default;

		void run();

		void setWindowSize(int vWidth, int vHeight) { m_WindowCreateInfo.WindowWidth = vWidth; m_WindowCreateInfo.WindowHeight = vHeight; }
		void setWindowPos(int vPosX, int vPosY) { m_WindowCreateInfo.WindowPosX = vPosX; m_WindowCreateInfo.WindowPosY = vPosY; }
		void setWindowFullScreen(bool vFullScreen) { m_WindowCreateInfo.IsWindowFullScreen = vFullScreen; }
		void setWindowResizable(bool vResizable) { m_WindowCreateInfo.IsWindowResizable = vResizable; }
		void setWindowTitle(const std::string& vTitle) { m_WindowCreateInfo.WindowTitle = vTitle; }

		CCamera*	fetchCamera() const { return m_pCamera; }
		GLFWwindow* fetchWindow() const { return m_pWindow; }
		CVkContext&	fetchVkContext() { return m_VkContext; }

		double getFrameInterval() const { return m_FrameInterval; }

		vk::PhysicalDeviceFeatures& fetchEnabledPhysicalDeviceFeatures() { return m_EnabledPhysicalDeviceFeatures; }

	protected:
		_DISALLOW_COPY_AND_ASSIGN(CVkApplicationBase);

		virtual void _awakeV() {}
		virtual bool _initV() { return true; }
		virtual void _updateV() {}
		virtual void _destroyV() {}

		vk::PhysicalDeviceFeatures _enabledPhysicalDeviceFeatures() const { return m_EnabledPhysicalDeviceFeatures; }

	protected:
		CVkContext m_VkContext;

	private:
		GLFWwindow* m_pWindow = nullptr;
		CCamera*	m_pCamera = nullptr;

		SWindowCreateInfo m_WindowCreateInfo = {};

		vk::PhysicalDeviceFeatures m_EnabledPhysicalDeviceFeatures = {};

		double	m_FrameInterval = 0.0;
		bool	m_IsInitialized = false;

		bool __initWindow();

		void __awake();
		bool __init();
		void __udpate();
		void __destroy();
	};
}