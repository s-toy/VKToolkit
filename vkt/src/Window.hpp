#include <iostream>
#include <GLFW/glfw3.h>
#include "Common.hpp"

namespace hiveVKT
{
	class CWindow
	{
	public:
		CWindow() {}
		~CWindow() { if (m_pWindow) glfwDestroyWindow(m_pWindow); }

		bool init(const SDisplayInfo& vDisplayInfo)
		{
			if (!glfwInit()) { _OUTPUT_WARNING("Fail to initialize window due to failure of glfwInit()!"); return false; }

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, vDisplayInfo.IsWindowResizable);
			glfwSetErrorCallback(__glfwErrorCallback);

			_ASSERTE(!m_pWindow);
			GLFWmonitor *pMonitor = vDisplayInfo.IsWindowFullScreen ? glfwGetPrimaryMonitor() : nullptr;
			m_pWindow = glfwCreateWindow(vDisplayInfo.WindowWidth, vDisplayInfo.WindowHeight, vDisplayInfo.WindowTitle.c_str(), pMonitor, nullptr);
			if (!m_pWindow)
			{
				std::cerr << "Fail to create window due to failure of glfwCreateWindow()! \n";
				glfwTerminate();
				return false;
			}

			glfwSetWindowPos(m_pWindow, vDisplayInfo.WindowPosX, vDisplayInfo.WindowPosY);

			return true;
		}

		GLFWwindow* getGLFWwindow() const { return m_pWindow; }

	private:
		GLFWwindow* m_pWindow = nullptr;

		static void __glfwErrorCallback(int vError, const char* vDescription)
		{
			std::cerr << "GLFW error: " << vDescription << std::endl;
		}
	};
}