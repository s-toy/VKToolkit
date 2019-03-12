#include "VulkanApp.h"
#include <iostream>

using namespace VulkanApp;

int main()
{
	CPerpixelShadingApp App;
	App.setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	App.setWindowPos(100, 100);

	try
	{
		App.run();
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}