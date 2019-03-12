#include "VulkanApp.h"
#include <iostream>

using namespace VulkanApp;

int main()
{
	CVulkanApp app;

	try
	{
		app.run();
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}