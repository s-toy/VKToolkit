#pragma once
#include <iostream>
#include <vulkan/vulkan.hpp>
#include "Window.hpp"

namespace hiveVKT
{
	class CWindow;

	class CVkApplicationBase
	{
	public:
		CVkApplicationBase() {}
		virtual ~CVkApplicationBase() {}

		void setWindowSize(int vWidth, int vHeight) { m_DisplayInfo.WindowWidth = vWidth; m_DisplayInfo.WindowHeight = vHeight; }
		void setWindowPos(int x, int y) { m_DisplayInfo.WindowPosX = x; m_DisplayInfo.WindowPosY = y; }
		void setWindowFullScreen(bool vFullScreen) { m_DisplayInfo.IsWindowFullScreen = vFullScreen; }
		void setWindowResizable(bool vResizable) { m_DisplayInfo.IsWindowResizable = vResizable; }

		bool run()
		{
			if (!_initV())
			{
				std::cerr << "Failed to run renderer due to failure of initialization!" << std::endl;
				return false;
			}

			bool IsNormalExit = true;
			try
			{
				while (!m_IsRenderLoopDone)
				{
					if (!_renderV())
					{
						std::cerr << "Render loop interrupted due to render failure!" << std::endl;
						IsNormalExit = false;
						break;
					}
					m_IsRenderLoopDone = _isRenderLoopDoneV();
				}
			}
			catch (...)
			{
				IsNormalExit = false;
			}

			return IsNormalExit;
		}

	protected:
		virtual bool _initV()
		{
			_ASSERTE(!m_pWindow);

			m_pWindow = new CWindow;
			if (!m_pWindow->init(m_DisplayInfo)) { std::cerr << "Fail to initialize renderer due to failure of initializing window!"; return false; }

			return true;
		}

		virtual bool _renderV()
		{
			_handleEventV();
			glfwPollEvents();

			return true;
		}

		virtual bool _isRenderLoopDoneV()
		{
			_ASSERTE(m_pWindow && m_pWindow->getGLFWwindow());

			bool IsRenderLoopDone = glfwWindowShouldClose(m_pWindow->getGLFWwindow());
			if (IsRenderLoopDone) { delete m_pWindow; glfwTerminate(); }

			return IsRenderLoopDone;
		}

		virtual void _handleEventV() {}

		virtual void _destroyV() {}

		GLFWwindow* _getGLFWwindow() const { return m_pWindow->getGLFWwindow(); }

	private:
		CWindow* m_pWindow = nullptr;
		SDisplayInfo m_DisplayInfo;

		bool m_IsInitialized = false;
		bool m_IsRenderLoopDone = false;
	};
}