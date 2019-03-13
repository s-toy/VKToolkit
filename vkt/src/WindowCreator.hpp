#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "Common.hpp"

namespace hiveVKT
{
	class CWindowCreator
	{
	public:
		GLFWwindow* create(const SDisplayInfo& vDisplayInfo)
		{
			_ASSERT_EXPR(vDisplayInfo.isValid(), "Input parameters for creating window must be valid!");

			if (!glfwInit()) { _OUTPUT_WARNING("Failed to create window due to failure of glfwInit()!"); return nullptr; }

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, vDisplayInfo.IsWindowResizable);

		#ifdef _ENABLE_DEBUG_UTILS
			glfwSetErrorCallback(__glfwErrorCallback);
		#endif

			GLFWmonitor *pMonitor = vDisplayInfo.IsWindowFullScreen ? glfwGetPrimaryMonitor() : nullptr;
			auto pWindow = glfwCreateWindow(vDisplayInfo.WindowWidth, vDisplayInfo.WindowHeight, vDisplayInfo.WindowTitle.c_str(), pMonitor, nullptr);
			if (!pWindow)
			{
				_OUTPUT_WARNING("Failed to create window due to failure of glfwCreateWindow()!");
				return nullptr;
			}

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
	};
}