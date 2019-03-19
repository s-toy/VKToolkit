#include <memory>
#include "VulkanApp.h"

using namespace VulkanApp;

//FUNCTION: detect the memory leak in DEBUG mode
void installMemoryLeakDetector()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 27464;
#endif
}

int main()
{
	installMemoryLeakDetector();

	CPerpixelShadingApp Application;

	Application.setWindowSize(1600, 900);
	Application.setWindowPos(100, 100);
	Application.setWindowTitle("perpixel shading demo");

	auto& PhsicalDeviceFeatures = Application.fetchPhysicalDeviceFeatures();
	PhsicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	PhsicalDeviceFeatures.sampleRateShading = VK_TRUE;

	Application.run();

	return EXIT_SUCCESS;
}