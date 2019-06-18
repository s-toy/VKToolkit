#include "pch.h"
#include "VkContext.h"

using namespace hiveVKT;

//TODO: ����ApiDump.txt��ȷ����������vulkan api

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
		CVkContext::getInstance()->setExtraFuncStatus(ENABLE_DEBUG_UTILS);
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

//���Ե㣺����Ĭ��VkContext
TEST_F(Test_VkContext, CreateDefaultContext)
{
	createContext();
	destroyContext();
}

//���Ե㣺�ظ�����VkContext
TEST_F(Test_VkContext, CreateContextTwice)
{
	createContext();
	createContext();
	destroyContext();
}

//���Ե㣺�ظ�����VkContext
TEST_F(Test_VkContext, DestroyContextTwice)
{
	createContext();
	destroyContext();
	destroyContext();
}

//���Ե㣺��δ�������������VkContext
TEST_F(Test_VkContext, DestroyContextWithoutCreation)
{
	destroyContext();
}

//���Ե㣺VkContext�������ܶ�̬����
TEST_F(Test_VkContext, ForBidModificationAfterContextCreated)
{
	createContext();
	
	CVkContext::getInstance()->setExtraFuncStatus(ENABLE_API_DUMP);
	EXPECT_EQ(false, isDeviceLayerEnabled("VK_LAYER_LUNARG_api_dump"));

	destroyContext();
}

//���Ե�: ����ѡ������Կ�
TEST_F(Test_VkContext, SetPreferDiscreteGPU)
{
	CVkContext::getInstance()->setExtraFuncStatus(PREFER_DISCRETE_GPU);
	createContext();
	EXPECT_EQ(vk::PhysicalDeviceType::eDiscreteGpu, CVkContext::getInstance()->getPhysicalDevice().getProperties().deviceType);
}

//���Ե㣺Ҫ��ѡ���GPU����֧��ͼ�ι���
TEST_F(Test_VkContext, ForceGraphicsFunction)
{
	CVkContext::getInstance()->setExtraFuncStatus(FORCE_GARPHICS_FUNCTIONALITY);
	createContext();
}

//���Ե㣺Ҫ��ѡ���GPU����֧�ּ��㹦��
TEST_F(Test_VkContext, ForceComputeFunction)
{
	CVkContext::getInstance()->setExtraFuncStatus(FORCE_COMPUTE_FUNCTIONALITY);
	createContext();
}

//���Ե㣺Ҫ��ѡ���GPU����֧�ִ��书��
TEST_F(Test_VkContext, ForceTransferFunction)
{
	CVkContext::getInstance()->setExtraFuncStatus(FORCE_TRANSFER_FUNCTIONALITY);
	createContext();
}

//���Ե㣺����Presentation֧��
TEST_F(Test_VkContext, EnablePresentation)
{
	CVkContext::getInstance()->setExtraFuncStatus(ENABLE_PRESENTATION);
	createContext();

	EXPECT_EQ(true, isDeviceExtensionEnabled("VK_KHR_win32_surface"));
	EXPECT_EQ(true, isDeviceExtensionEnabled(VK_KHR_SURFACE_EXTENSION_NAME));
}

//���Ե㣺����Api���ü�¼֧��
TEST_F(Test_VkContext, EnableApiDump)
{
	CVkContext::getInstance()->setExtraFuncStatus(ENABLE_API_DUMP);
	createContext();

	EXPECT_EQ(true, isDeviceLayerEnabled("VK_LAYER_LUNARG_api_dump"));
}

//���Ե㣺����֡�ʼ�¼֧��
TEST_F(Test_VkContext, EnableFpsMonitor)
{
	CVkContext::getInstance()->setExtraFuncStatus(ENABLE_FPS_MONITOR);
	createContext();

	EXPECT_EQ(true, isDeviceLayerEnabled("VK_LAYER_LUNARG_monitor"));
}

//���Ե㣺������Ļ��ͼ֧��
TEST_F(Test_VkContext, EnableScreenshot)
{
	CVkContext::getInstance()->setExtraFuncStatus(ENABLE_SCREENSHOT);
	createContext();

	EXPECT_EQ(true, isDeviceLayerEnabled("VK_LAYER_LUNARG_screenshot"));
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