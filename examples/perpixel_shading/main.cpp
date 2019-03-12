#include "VulkanApp.h"
#include <iostream>

using namespace VulkanApp;

int main()
{
	CPerpixelShadingApp App;
	App.setWindowSize(1600, 900);
	App.setWindowPos(100, 100);
	App.setWindowTitle("perpixel shading demo");

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