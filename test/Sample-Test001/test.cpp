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

//���Ե㣺����Ĭ��VkContext
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

//���Ե㣺�ظ�����VkContext
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

//���Ե㣺�ظ�����VkContext
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

	dumpVkAPIAt(0, 0);
}

//���Ե�: ����ѡ������Կ�
TEST_F(Test_VkContext, SetPreferDiscreteGPU)
{
	CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	createContext();
	EXPECT_EQ(vk::PhysicalDeviceType::eDiscreteGpu, CVkContext::getInstance()->getPhysicalDevice().getProperties().deviceType);
}

//���Ե㣺Ҫ��ѡ���GPU����֧��ͼ�ι���
TEST_F(Test_VkContext, ForceGraphicsFunction)
{
	CVkContext::getInstance()->setForceGraphicsFunctionalityHint(true);
	createContext();
}

//���Ե㣺Ҫ��ѡ���GPU����֧�ּ��㹦��
TEST_F(Test_VkContext, ForceComputeFunction)
{
	CVkContext::getInstance()->setForceComputeFunctionalityHint(true);
	createContext();
}

//���Ե㣺Ҫ��ѡ���GPU����֧�ִ��书��
TEST_F(Test_VkContext, ForceTransferFunction)
{
	CVkContext::getInstance()->setForceTransferFunctionalityHint(true);
	createContext();
}

//���Ե㣺����Presentation֧��
TEST_F(Test_VkContext, EnablePresentation)
{
	CVkContext::getInstance()->setEnablePresentationHint(true);
	createContext();
}

//���Ե㣺����Api���ü�¼֧��
TEST_F(Test_VkContext, EnableApiDump)
{
	CVkContext::getInstance()->setEnablePresentationHint(true);
	createContext();

}

//���Ե㣺����֡�ʼ�¼֧��
TEST_F(Test_VkContext, EnableFpsMonitor)
{
	CVkContext::getInstance()->setEnableFpsMonitorHint(true);
	createContext();

}

//���Ե㣺������Ļ��ͼ֧��
TEST_F(Test_VkContext, EnableScreenshot)
{
	CVkContext::getInstance()->setEnableScreenshotHint(true);
	createContext();

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