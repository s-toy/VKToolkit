#include "pch.h"
#include "VkContext.h"
#include "VkCallParser.h"

using namespace hiveVKT;

class Test_VkContext : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		remove("vk_apidump.txt");
	}

	void createContext()
	{
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		CVkContext::getInstance()->setEnableApiDumpHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());

		m_pDebugUtilsMessenger = &(CVkContext::getInstance()->getDebugUtilsMessenger());
		EXPECT_EQ(0, m_pDebugUtilsMessenger->getWarningAndErrorCount());
	}

	void destroyContext()
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	void dumpVkAPIAt(int vThread, int vFrame)
	{
		CVkCallParser Parser;
		bool r = Parser.parse("vk_apidump.txt");
		ASSERT_EQ(r, true);
		m_VkCallInfoSet = Parser.getVKCallInfoAt(vThread, vFrame);
	}

	std::vector<SVKCallInfo> verifyAndFetchInvokedVkCall(const std::string& vFunctionName, const std::string& vReturnValue, int vInvokeTimes)
	{
		std::vector<SVKCallInfo> InvokedVkCallSet;
		for (const auto& VkCallInfo : m_VkCallInfoSet)
		{
			if (VkCallInfo.FunctionName == vFunctionName && VkCallInfo.ReturnValue == vReturnValue)
			{
				vInvokeTimes--;
				InvokedVkCallSet.push_back(VkCallInfo);
			}
		}
		EXPECT_EQ(vInvokeTimes, 0);
		return InvokedVkCallSet;
	}

	void verifyVKCallParameter()
	{

	}

	const CVkDebugUtilsMessenger* m_pDebugUtilsMessenger = nullptr;
	std::vector<SVKCallInfo> m_VkCallInfoSet;
};

//测试点：创建默认VkContext
TEST_F(Test_VkContext, CreateDefaultContext)
{
	createContext();
	destroyContext();

	dumpVkAPIAt(0, 0);
	verifyAndFetchInvokedVkCall("vkCreateInstance", "VK_SUCCESS", 1);
	verifyAndFetchInvokedVkCall("vkCreateDevice", "VK_SUCCESS", 1);
	verifyAndFetchInvokedVkCall("vkDestroyDevice", "void", 1);
	verifyAndFetchInvokedVkCall("vkDestroyInstance", "void", 1);
}

//测试点：重复创建VkContext
TEST_F(Test_VkContext, CreateContextTwice)
{
	createContext();
	createContext();
	destroyContext();

	dumpVkAPIAt(0, 0);
	verifyAndFetchInvokedVkCall("vkCreateInstance", "VK_SUCCESS", 1);
	verifyAndFetchInvokedVkCall("vkCreateDevice", "VK_SUCCESS", 1);
	verifyAndFetchInvokedVkCall("vkDestroyDevice", "void", 1);
	verifyAndFetchInvokedVkCall("vkDestroyInstance", "void", 1);
}

//测试点：重复析构VkContext
TEST_F(Test_VkContext, DestroyContextTwice)
{
	createContext();
	destroyContext();
	destroyContext();
	
	dumpVkAPIAt(0, 0);
	verifyAndFetchInvokedVkCall("vkCreateInstance", "VK_SUCCESS", 1);
	verifyAndFetchInvokedVkCall("vkCreateDevice", "VK_SUCCESS", 1);
	verifyAndFetchInvokedVkCall("vkDestroyDevice", "void", 1);
	verifyAndFetchInvokedVkCall("vkDestroyInstance", "void", 1);
}

//测试点：在未创建情况下析构VkContext
TEST_F(Test_VkContext, DestroyContextWithoutCreation)
{
	destroyContext();

	CVkCallParser Parser;
	bool r = Parser.parse("vk_apidump.txt");
	EXPECT_EQ(r, false);
}

//测试点：VkContext创建后不能动态更改
TEST_F(Test_VkContext, ForBidModificationAfterContextCreated)
{
	createContext();
	CVkContext::getInstance()->setApplicationName("HelloVulkan");
	destroyContext();

	dumpVkAPIAt(0, 0);
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
}

//测试点：开启Api调用记录支持
TEST_F(Test_VkContext, EnableApiDump)
{
	CVkContext::getInstance()->setEnablePresentationHint(true);
	createContext();

}

//测试点：开启帧率记录支持
TEST_F(Test_VkContext, EnableFpsMonitor)
{
	CVkContext::getInstance()->setEnableFpsMonitorHint(true);
	createContext();

}

//测试点：开启屏幕截图支持
TEST_F(Test_VkContext, EnableScreenshot)
{
	CVkContext::getInstance()->setEnableScreenshotHint(true);
	createContext();

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