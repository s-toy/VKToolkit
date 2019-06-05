#include "pch.h"
#include "VkContext.h"

using namespace hiveVKT;

//TODO: 利用ApiDump.txt来确保正常调用vulkan api

class Test_VkContext : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
	}

	virtual void TearDown() override
	{
	}

	void createContext()
	{
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());

		m_pDebugUtilsMessenger = &(CVkContext::getInstance()->getDebugUtilsMessenger());
		EXPECT_EQ(0, m_pDebugUtilsMessenger->getWarningAndErrorCount());
	}

	void destroyContext()
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	bool isDeviceExtensionEnabled(const std::string& vExtension)
	{
		auto EnabledDeviceLayers = CVkContext::getInstance()->fetchEnabledDeviceLayers();
		for (auto Layer : EnabledDeviceLayers)
		{
			//TODO
		}
		return true;
	}

	bool isDeviceLayerEnabled(const std::string& vLayer)
	{
		auto EnabledDeviceLayers = CVkContext::getInstance()->fetchEnabledDeviceLayers();
		for (auto Layer : EnabledDeviceLayers)
		{
			//TODO
		}
		return true;
	}

	const CVkDebugUtilsMessenger* m_pDebugUtilsMessenger = nullptr;
};

//测试点：创建默认VkContext
TEST_F(Test_VkContext, CreateDefaultContext)
{
	createContext();
	destroyContext();
}

//测试点：重复创建VkContext
TEST_F(Test_VkContext, CreateContextTwice)
{
	createContext();
	createContext();
	destroyContext();
}

//测试点：重复析构VkContext
TEST_F(Test_VkContext, DestroyContextTwice)
{
	createContext();
	destroyContext();
	destroyContext();
}

//测试点：在未创建情况下析构VkContext
TEST_F(Test_VkContext, DestroyContextWithoutCreation)
{
	destroyContext();
}

//测试点：VkContext创建后不能动态更改
TEST_F(Test_VkContext, ForBidModificationAfterContextCreated)
{
	createContext();
	
	CVkContext::getInstance()->setEnableApiDumpHint(true);
	EXPECT_EQ(false, isDeviceLayerEnabled("VK_LAYER_LUNARG_api_dump"));

	destroyContext();
}

//测试点: 倾向选择独立显卡
TEST_F(Test_VkContext, SetPreferDiscreteGPU)
{
	CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	createContext();
	EXPECT_EQ(vk::PhysicalDeviceType::eDiscreteGpu, CVkContext::getInstance()->getPhysicalDevice().getProperties().deviceType);
}

//测试点：要求选择的GPU必须支持图形功能
TEST_F(Test_VkContext, ForceGraphicsFunction)
{
	CVkContext::getInstance()->setForceGraphicsFunctionalityHint(true);
	createContext();
}

//测试点：要求选择的GPU必须支持计算功能
TEST_F(Test_VkContext, ForceComputeFunction)
{
	CVkContext::getInstance()->setForceComputeFunctionalityHint(true);
	createContext();
}

//测试点：要求选择的GPU必须支持传输功能
TEST_F(Test_VkContext, ForceTransferFunction)
{
	CVkContext::getInstance()->setForceTransferFunctionalityHint(true);
	createContext();
}

//测试点：开启Presentation支持
TEST_F(Test_VkContext, EnablePresentation)
{
	CVkContext::getInstance()->setEnablePresentationHint(true);
	createContext();

	EXPECT_EQ(true, isDeviceExtensionEnabled("VK_KHR_win32_surface"));
	EXPECT_EQ(true, isDeviceExtensionEnabled(VK_KHR_SURFACE_EXTENSION_NAME));
}

//测试点：开启Api调用记录支持
TEST_F(Test_VkContext, EnableApiDump)
{
	CVkContext::getInstance()->setEnablePresentationHint(true);
	createContext();

	EXPECT_EQ(true, isDeviceLayerEnabled("VK_LAYER_LUNARG_api_dump"));
}

//测试点：开启帧率记录支持
TEST_F(Test_VkContext, EnableFpsMonitor)
{
	CVkContext::getInstance()->setEnableFpsMonitorHint(true);
	createContext();

	EXPECT_EQ(true, isDeviceLayerEnabled("VK_LAYER_LUNARG_monitor"));
}

//测试点：开启屏幕截图支持
TEST_F(Test_VkContext, EnableScreenshot)
{
	CVkContext::getInstance()->setEnableScreenshotHint(true);
	createContext();

	EXPECT_EQ(true, isDeviceLayerEnabled("VK_LAYER_LUNARG_screenshot"));
}

//TODO: 
//测试点：开启设备不支持的扩展
TEST_F(Test_VkContext, EnableUnsupportedExtension)
{	
}

//测试点：开启设备支持的扩展
TEST_F(Test_VkContext, EnableSupportedExtension)
{
}

//测试点：开启设备不支持的特性
TEST_F(Test_VkContext, EnableUnsupportedFeature)
{
}

//测试点：开启设备支持的特性
TEST_F(Test_VkContext, EnableSupportedFeature)
{
}