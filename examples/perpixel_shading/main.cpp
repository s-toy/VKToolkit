#include <memory>
#include "VkForwardApplication.hpp"

using namespace hiveVKT;

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

	CVkForwardApplication Application;
	Application.setWindowSize(1600, 900);
	Application.setWindowPos(100, 100);
	Application.setWindowTitle("perpixel shading demo");

	auto& PhsicalDeviceFeatures = Application.fetchPhysicalDeviceFeatures();
	PhsicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	PhsicalDeviceFeatures.sampleRateShading = VK_TRUE;

	hiveVKT::SVertexLayout VertexLayout;
	VertexLayout.ComponentSet.push_back(hiveVKT::EVertexComponent::VERTEX_COMPONENT_POSITION);
	VertexLayout.ComponentSet.push_back(hiveVKT::EVertexComponent::VERTEX_COMPONENT_NORMAL);
	VertexLayout.ComponentSet.push_back(hiveVKT::EVertexComponent::VERTEX_COMPONENT_TEXCOORD);

	hiveVKT::STextureDescriptorBindingInfo TextureDescriptorBindingInfo;
	TextureDescriptorBindingInfo.TextureDescriptorBindingInfo.push_back({ hiveVKT::ETextureType::TEXTURE_TYPE_DIFF, 0 });
	TextureDescriptorBindingInfo.TextureDescriptorBindingInfo.push_back({ hiveVKT::ETextureType::TEXTURE_TYPE_SPEC, 1 });

	auto ModelID = Application.loadModel({}, VertexLayout, TextureDescriptorBindingInfo);

	auto UpdateFunc = [&]() {
		auto ViewInfo = Application.getViewInfo();

		std::vector<std::weak_ptr<SMesh>> MeshBufferSet;
		Application.updateModelData(ModelID, MeshBufferSet);
	};

	Application.registerUpdateFunction(UpdateFunc);

	Application.run();

	return EXIT_SUCCESS;
}