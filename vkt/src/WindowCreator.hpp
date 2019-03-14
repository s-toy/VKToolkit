#pragma once
#include <GLFW/glfw3.h>
#include "Common.hpp"

#ifdef _WINDOWS
#include <windows.h>
#endif // _WINDOWS

namespace hiveVKT
{
	class CWindowCreator
	{
	public:
		GLFWwindow* create(const SDisplayInfo& vDisplayInfo)
		{
			_HIVE_EARLY_RETURN(!vDisplayInfo.isValid(), "Input parameters for creating window must be valid!", nullptr);

			_HIVE_EARLY_RETURN(!glfwInit(), "Failed to create window due to failure of glfwInit()!", nullptr);

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, vDisplayInfo.IsWindowResizable);

		#ifdef _ENABLE_DEBUG_UTILS
			glfwSetErrorCallback(__glfwErrorCallback);
		#endif

			auto pWindow = vDisplayInfo.IsWindowFullScreen ? __createFullScreenWindow(vDisplayInfo) : 
				glfwCreateWindow(vDisplayInfo.WindowWidth, vDisplayInfo.WindowHeight, vDisplayInfo.WindowTitle.c_str(), nullptr, nullptr);

			_HIVE_EARLY_RETURN(!pWindow, "Failed to create window due to failure of glfwCreateWindow()!", nullptr);

			glfwSetWindowPos(pWindow, vDisplayInfo.WindowPosX, vDisplayInfo.WindowPosY);

			return pWindow;
		}

	private:
#ifdef _ENABLE_DEBUG_UTILS
		static void __glfwErrorCallback(int vError, const char* vDescription)
		{
			_OUTPUT_WARNING(std::string("GLFW error: ") + vDescription);
		}
#endif

		static bool __queryScreenSize(int& voScreenWidth, int& voScreenHeight)
		{
		#ifdef _WINDOWS
			voScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			voScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		#else
			_OUTPUT_WARNING("Failed to query screen size due to the platform is not supported!");
			return false;
		#endif // _WINDOWS

			return true;
		}

		static GLFWwindow* __createFullScreenWindow(const SDisplayInfo& vDisplayInfo)
		{
			GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();

			int ScreenWidth, ScreenHeight;
			__queryScreenSize(ScreenWidth, ScreenHeight);

			return glfwCreateWindow(ScreenWidth, ScreenHeight, vDisplayInfo.WindowTitle.c_str(), pMonitor, nullptr);
		}
	};
}