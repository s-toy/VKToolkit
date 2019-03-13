#include "VulkanApp.h"
#include <iostream>

using namespace VulkanApp;

int main()
{
	CPerpixelShadingApp Application;

	Application.setWindowSize(1600, 900);
	Application.setWindowPos(100, 100);
	Application.setWindowTitle("perpixel shading demo");

	Application.run();

	return EXIT_SUCCESS;
}