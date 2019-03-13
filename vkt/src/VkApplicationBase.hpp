#pragma once
#include <vulkan/vulkan.hpp>
#include "Window.hpp"
#include "Utility.hpp"
#include "Common.hpp"
#include "VkInstanceCreator.hpp"
#include "VkDeviceCreator.hpp"
#include "VkDebugMessenger.hpp"

namespace hiveVKT
{
	class CWindow;

	class CVkApplicationBase
	{
	public:
		CVkApplicationBase() = default;
		virtual ~CVkApplicationBase() = default;

		void setWindowSize(int vWidth, int vHeight) { m_DisplayInfo.WindowWidth = vWidth; m_DisplayInfo.WindowHeight = vHeight; }
		void setWindowPos(int vPosX, int vPosY) { m_DisplayInfo.WindowPosX = vPosX; m_DisplayInfo.WindowPosY = vPosY; }
		void setWindowFullScreen(bool vFullScreen) { m_DisplayInfo.IsWindowFullScreen = vFullScreen; }
		void setWindowResizable(bool vResizable) { m_DisplayInfo.IsWindowResizable = vResizable; }
		void setWindowTitle(const std::string& vTitle) { m_DisplayInfo.WindowTitle = vTitle; }

		void run()
		{
			try
			{
				if (!_initV()) { _THROW_RUNTINE_ERROR("Failed to run application due to failure of initialization!"); }

				_OUTPUT_EVENT("Succeed to init application.");

				while (!m_IsRenderLoopDone)
				{
					if (!_renderV()) { _THROW_RUNTINE_ERROR("Render loop interrupted due to render failure!"); }
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

	protected:
		virtual bool _initV()
		{
			if (!__initWindow()) { _OUTPUT_WARNING("Failed to initialize application due to failure of initializing window!"); return false; }
			if (!__initVulkan()) { _OUTPUT_WARNING("Failed to initialize application due to failure of initializing vulkan!"); return false; }

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
			if (IsRenderLoopDone) { _SAFE_DELETE(m_pWindow); glfwTerminate(); }

			return IsRenderLoopDone;
		}

		virtual void _handleEventV() {}

		virtual void _destroyV()
		{
			m_DebugMessenger.destroyDebugMessenger(m_VkInstance);
			m_VkInstance.destroySurfaceKHR(m_VkSurface);
			m_VkInstance.destroy();

			_SAFE_DELETE(m_pWindow);
			glfwTerminate();
		}

		GLFWwindow* _window() const { return m_pWindow->getGLFWwindow(); }

		vk::Instance _instance() const { return m_VkInstance; }
		vk::SurfaceKHR _surface() const { return m_VkSurface; }

	private:
		CWindow* m_pWindow = nullptr;
		SDisplayInfo m_DisplayInfo = {};

		CVkDebugMessenger m_DebugMessenger;

		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;

		vk::Instance m_VkInstance;
		vk::PhysicalDevice m_VkPhysicalDevice;

		bool m_IsInitialized = false;
		bool m_IsRenderLoopDone = false;

		bool __initWindow()
		{
			_ASSERTE(!m_pWindow);
			m_pWindow = new CWindow;
			return m_pWindow->init(m_DisplayInfo);
		}

		bool __initVulkan()
		{
			__createInstance();
			__createSurface();
			__pickPhysicalDevice();

			m_DebugMessenger.setupDebugMessenger(m_VkInstance);
		}

		void __createInstance() { hiveVKT::CVkInstanceCreator InstanceCreator; m_VkInstance = InstanceCreator.create(); }

		void __createSurface()
		{
			if (glfwCreateWindowSurface(m_VkInstance, _window(), nullptr, &m_VkSurface) != VK_SUCCESS) _THROW_RUNTINE_ERROR("Failed to create window surface!");
		}

		void __pickPhysicalDevice() {}
	};
}