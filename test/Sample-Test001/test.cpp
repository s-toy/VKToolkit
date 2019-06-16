#include "pch.h"
#include <set>
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

		EXPECT_EQ(0, CVkContext::getInstance()->getWarningAndErrorCount());
	}

	void destroyContext()
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	void dumpVkAPI()
	{
		bool r = m_VkCallParser.parse("vk_apidump.txt");
		ASSERT_EQ(r, true);
	}

	void verifyInvokedVkCallInfo(int vThreadID, int vFrameID, const std::string& vFunctionName, const std::vector<std::string>& vReturnValue)
	{
		auto FunctionCallInfo = m_VkCallParser.getVkCallInfoByFunctionName(vThreadID, vFrameID, vFunctionName);

		EXPECT_EQ(FunctionCallInfo.size(), vReturnValue.size());

		for (auto i = 0; i < FunctionCallInfo.size(); ++i)
			EXPECT_EQ(FunctionCallInfo[i].ReturnValue, vReturnValue[i]);
	}

	void verifyVkCallParameter(int vThreadID, int vFrameID, const std::string& vFunctionName, const std::vector<std::pair<std::string, std::string>>& vExpectedParameters)
	{
		auto FunctionCallInfo = m_VkCallParser.getVkCallInfoByFunctionName(vThreadID, vFrameID, vFunctionName);
		EXPECT_EQ(FunctionCallInfo.size(), 1);

		auto DumpedParameterInfo = FunctionCallInfo[0].ParameterInfo;

		for (auto i = 0; i < vExpectedParameters.size(); ++i)
			EXPECT_EQ(DumpedParameterInfo[vExpectedParameters[i].first].second, vExpectedParameters[i].second);
	}

	void verifyQueueFlags(const vk::QueueFlagBits& vExpectedFlagBit)
	{
		auto QueueFamilyIndex = CVkContext::getInstance()->getComprehensiveQueueFamilyIndex();

		auto QueueFamilyProperties = CVkContext::getInstance()->getPhysicalDevice().getQueueFamilyProperties();

		EXPECT_TRUE(QueueFamilyProperties[QueueFamilyIndex].queueFlags & vExpectedFlagBit);
	}

	void verifyEnabledLayersOrExtensions(const std::string& vFunctionName, const std::string& vPrefix, const std::vector<std::string>& vExpectedLayersOrExtensions)
	{
		auto FunctionCallInfo = m_VkCallParser.getVkCallInfoByFunctionName(0, 0, vFunctionName);
		EXPECT_EQ(FunctionCallInfo.size(), 1);

		auto DumpedParameterInfo = FunctionCallInfo[0].ParameterInfo;

		std::set<std::string> Actual;

		for (auto Item : DumpedParameterInfo)
		{
			if (Item.first.find(vPrefix) != std::string::npos)
				Actual.insert(Item.second.second);
		}

		for (auto& Item : vExpectedLayersOrExtensions)
			EXPECT_TRUE(Actual.find(Item) != Actual.end());
	}

	CVkCallParser m_VkCallParser;
};

//���Ե㣺����Ĭ��VkContext
TEST_F(Test_VkContext, CreateDefaultContext)
{
	createContext();
	destroyContext();

	dumpVkAPI();

	verifyInvokedVkCallInfo(0, 0, "vkCreateInstance", { "VK_SUCCESS" });
	verifyInvokedVkCallInfo(0, 0, "vkCreateDevice", { "VK_SUCCESS" });
	verifyInvokedVkCallInfo(0, 0, "vkDestroyDevice", { "void" });
	verifyInvokedVkCallInfo(0, 0, "vkDestroyInstance", { "void" });
}

//���Ե㣺�ظ�����VkContext
TEST_F(Test_VkContext, CreateContextTwice)
{
	createContext();
	createContext();
	destroyContext();

	dumpVkAPI();

	verifyInvokedVkCallInfo(0, 0, "vkCreateInstance", { "VK_SUCCESS" });
	verifyInvokedVkCallInfo(0, 0, "vkCreateDevice", { "VK_SUCCESS" });
	verifyInvokedVkCallInfo(0, 0, "vkDestroyDevice", { "void" });
	verifyInvokedVkCallInfo(0, 0, "vkDestroyInstance", { "void" });
}

//���Ե㣺�ظ�����VkContext
TEST_F(Test_VkContext, DestroyContextTwice)
{
	createContext();
	destroyContext();
	destroyContext();

	dumpVkAPI();

	verifyInvokedVkCallInfo(0, 0, "vkCreateInstance", { "VK_SUCCESS" });
	verifyInvokedVkCallInfo(0, 0, "vkCreateDevice", { "VK_SUCCESS" });
	verifyInvokedVkCallInfo(0, 0, "vkDestroyDevice", { "void" });
	verifyInvokedVkCallInfo(0, 0, "vkDestroyInstance", { "void" });
}

//���Ե㣺��δ�������������VkContext
TEST_F(Test_VkContext, DestroyContextWithoutCreation)
{
	destroyContext();

	CVkCallParser Parser;
	bool r = Parser.parse("vk_apidump.txt");
	EXPECT_EQ(r, false);
}

//���Ե㣺VkContext�������ܶ�̬����
TEST_F(Test_VkContext, ForBidModificationAfterContextCreated)
{
	createContext();
	CVkContext::getInstance()->setApplicationName("HelloVulkan");
	destroyContext();

	dumpVkAPI();

	verifyInvokedVkCallInfo(0, 0, "vkCreateInstance", { "VK_SUCCESS" });
	verifyInvokedVkCallInfo(0, 0, "vkCreateDevice", { "VK_SUCCESS" });
	verifyInvokedVkCallInfo(0, 0, "vkDestroyDevice", { "void" });
	verifyInvokedVkCallInfo(0, 0, "vkDestroyInstance", { "void" });

	verifyVkCallParameter(0, 0, "vkCreateInstance", { {"pCreateInfo|pApplicationInfo|pApplicationName","\"Application\""},{"pCreateInfo|pApplicationInfo|pEngineName","\"HiveVKT\""} });
}

//���Ե�: ����ѡ������Կ�
TEST_F(Test_VkContext, SetPreferDiscreteGPU)
{
	CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	createContext();
	EXPECT_EQ(vk::PhysicalDeviceType::eDiscreteGpu, CVkContext::getInstance()->getPhysicalDevice().getProperties().deviceType);
	destroyContext();
}

//���Ե㣺Ҫ��ѡ���GPU����֧��ͼ�ι���
TEST_F(Test_VkContext, ForceGraphicsFunction)
{
	CVkContext::getInstance()->setForceGraphicsFunctionalityHint(true);
	createContext();
	verifyQueueFlags(vk::QueueFlagBits::eGraphics);
	destroyContext();
}

//���Ե㣺Ҫ��ѡ���GPU����֧�ּ��㹦��
TEST_F(Test_VkContext, ForceComputeFunction)
{
	CVkContext::getInstance()->setForceComputeFunctionalityHint(true);
	createContext();
	verifyQueueFlags(vk::QueueFlagBits::eCompute);
	destroyContext();
}

//���Ե㣺Ҫ��ѡ���GPU����֧�ִ��书��
TEST_F(Test_VkContext, ForceTransferFunction)
{
	CVkContext::getInstance()->setForceTransferFunctionalityHint(true);
	createContext();
	verifyQueueFlags(vk::QueueFlagBits::eTransfer);
	destroyContext();
}

//���Ե㣺����Presentation֧��
TEST_F(Test_VkContext, EnablePresentation)
{
	CVkContext::getInstance()->setEnablePresentationHint(true);
	createContext();
	destroyContext();

	dumpVkAPI();

	verifyEnabledLayersOrExtensions("vkCreateInstance", "pCreateInfo|ppEnabledExtensionNames|ppEnabledExtensionNames", { "\"VK_KHR_surface\"" ,"\"VK_KHR_win32_surface\"" });
	verifyEnabledLayersOrExtensions("vkCreateDevice", "pCreateInfo|ppEnabledExtensionNames|ppEnabledExtensionNames", { "\"VK_KHR_swapchain\"" });
}

//���Ե㣺����Api���ü�¼֧��
TEST_F(Test_VkContext, EnableApiDump)
{
	CVkContext::getInstance()->setEnablePresentationHint(true);
	createContext();
	destroyContext();

	dumpVkAPI();

	verifyEnabledLayersOrExtensions("vkCreateInstance", "pCreateInfo|ppEnabledLayerNames|ppEnabledLayerNames", { "\"VK_LAYER_LUNARG_api_dump\"" });
}

//���Ե㣺����֡�ʼ�¼֧��
TEST_F(Test_VkContext, EnableFpsMonitor)
{
	CVkContext::getInstance()->setEnableFpsMonitorHint(true);
	createContext();
	destroyContext();

	dumpVkAPI();

	verifyEnabledLayersOrExtensions("vkCreateInstance", "pCreateInfo|ppEnabledLayerNames|ppEnabledLayerNames", { "\"VK_LAYER_LUNARG_monitor\"" });

}

//���Ե㣺������Ļ��ͼ֧��
TEST_F(Test_VkContext, EnableScreenshot)
{
	CVkContext::getInstance()->setEnableScreenshotHint(true);
	createContext();
	destroyContext();

	dumpVkAPI();

	verifyEnabledLayersOrExtensions("vkCreateInstance", "pCreateInfo|ppEnabledLayerNames|ppEnabledLayerNames", { "\"VK_LAYER_LUNARG_screenshot\"" });
}

//TODO: 
//���Ե㣺�����豸��֧�ֵ���չ
TEST_F(Test_VkContext, EnableUnsupportedExtension)
{
}

//���Ե㣺�����豸֧�ֵ���չ
TEST_F(Test_VkContext, EnableSupportedExtension)
{
}

//���Ե㣺�����豸��֧�ֵ�����
TEST_F(Test_VkContext, EnableUnsupportedFeature)
{
}

//���Ե㣺�����豸֧�ֵ�����
TEST_F(Test_VkContext, EnableSupportedFeature)
{
}