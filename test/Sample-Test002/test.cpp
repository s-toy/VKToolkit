#include "pch.h"
#include <vulkan/vulkan.hpp>
#include "GLFW/glfw3.h"
#include "VkContext.h"
#include "VkSwapchain.h"
#include "VkCallParser.h"

#define CREATE_WINDOW glfwCreateWindow(800, 600, "Test_Swapchain", nullptr, nullptr)

class Test_VkSwapchain :public ::testing::Test
{
public:
	virtual void SetUp() override
	{
		hiveVKT::CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
		hiveVKT::CVkContext::getInstance()->setEnablePresentationHint(true);
		hiveVKT::CVkContext::getInstance()->createContext();

		glfwInit();
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_pWindow = CREATE_WINDOW;

		vk::SurfaceKHR pSurface = nullptr;
		glfwCreateWindowSurface(hiveVKT::CVkContext::getInstance()->getVulkanInstance(), m_pWindow, nullptr, reinterpret_cast<VkSurfaceKHR*>(&pSurface));

		vk::SurfaceCapabilitiesKHR SurfaceCapabilities = hiveVKT::CVkContext::getInstance()->getPhysicalDevice().getSurfaceCapabilitiesKHR(pSurface);
		m_SupportedImageUsages = SurfaceCapabilities.supportedUsageFlags;

		hiveVKT::CVkContext::getInstance()->getVulkanInstance().destroySurfaceKHR(pSurface);
		hiveVKT::CVkContext::getInstance()->destroyContext();
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

protected:
	vk::ImageUsageFlags m_SupportedImageUsages = vk::ImageUsageFlags();
	GLFWwindow* m_pWindow = nullptr;
	hiveVKT::CVkSwapchain m_Swapchain;
};

//���Ե㣺���Դ���մ���ָ��
//
TEST_F(Test_VkSwapchain, CreateSwapchainWithNullWindow)
{
	hiveVKT::CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	hiveVKT::CVkContext::getInstance()->setEnablePresentationHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableDebugUtilsHint(true);
	hiveVKT::CVkContext::getInstance()->createContext();

	EXPECT_FALSE(m_Swapchain.createSwapchain(nullptr, m_SupportedImageUsages));
	
	m_Swapchain.destroySwapchain();
	hiveVKT::CVkContext::getInstance()->destroyContext();
}

//���Ե㣺�����������
//
TEST_F(Test_VkSwapchain, CreateSwapchainWithValidWindow)
{
	hiveVKT::CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	hiveVKT::CVkContext::getInstance()->setEnablePresentationHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableDebugUtilsHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableApiDumpHint(true);
	hiveVKT::CVkContext::getInstance()->createContext();

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_pWindow = CREATE_WINDOW;

	EXPECT_TRUE(m_Swapchain.createSwapchain(m_pWindow, m_SupportedImageUsages));

	m_Swapchain.destroySwapchain();
	hiveVKT::CVkContext::getInstance()->destroyContext();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();

	hiveVKT::CVkCallParser Parser;
	EXPECT_TRUE(Parser.parse("vk_apidump.txt"));

	auto CreateSwapChainCallInfo = Parser.getVkCallInfoByFunctionName(0, 0, "vkCreateSwapchainKHR");
	EXPECT_EQ(CreateSwapChainCallInfo.size(), 1);
	EXPECT_EQ(CreateSwapChainCallInfo[0].ReturnValue, "VK_SUCCESS");
	EXPECT_EQ(atoi(CreateSwapChainCallInfo[0].ParameterInfo["pCreateInfo|imageUsage"].second.data()), (VkImageUsageFlags)m_SupportedImageUsages);
}

//���Ե㣺�����ؽ�swap chain
//
TEST_F(Test_VkSwapchain, RecreateSwapchain)
{
	//TODO
}

//���Ե㣺����ʹ�÷Ƿ���image usage flags����ʱ�����������������
//
TEST_F(Test_VkSwapchain, CreateWithInvalidImageUsageFlags)
{
	hiveVKT::CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	hiveVKT::CVkContext::getInstance()->setEnablePresentationHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableDebugUtilsHint(true);
	hiveVKT::CVkContext::getInstance()->createContext();

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_pWindow = CREATE_WINDOW;

	EXPECT_FALSE(m_Swapchain.createSwapchain(m_pWindow, ~m_SupportedImageUsages));

	m_Swapchain.destroySwapchain();
	hiveVKT::CVkContext::getInstance()->destroyContext();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

//���Ե㣺�����ظ�����create���������������������
//
TEST_F(Test_VkSwapchain, DuplicateInterfaceCall)
{
	hiveVKT::CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	hiveVKT::CVkContext::getInstance()->setEnablePresentationHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableDebugUtilsHint(true);
	hiveVKT::CVkContext::getInstance()->createContext();

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_pWindow = CREATE_WINDOW;

	EXPECT_TRUE(m_Swapchain.createSwapchain(m_pWindow, m_SupportedImageUsages));
	EXPECT_TRUE(m_Swapchain.createSwapchain(m_pWindow, m_SupportedImageUsages));

	m_Swapchain.destroySwapchain();
	m_Swapchain.destroySwapchain();

	hiveVKT::CVkContext::getInstance()->destroyContext();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

//���Ե㣺������δ��ʼ��Context֮ǰ����swap chainʱ�����������������
//
TEST_F(Test_VkSwapchain, CreateBeforeInitializeContext)
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_pWindow = CREATE_WINDOW;

	EXPECT_FALSE(m_Swapchain.createSwapchain(m_pWindow, m_SupportedImageUsages));
	m_Swapchain.destroySwapchain();
}

//���Ե㣺�����ڲ����ʵ��������д���swap chain��������԰�Ԥ�ڴ���
//
TEST_F(Test_VkSwapchain, CreateWithUnsuitableContext)
{
	hiveVKT::CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableDebugUtilsHint(true);
	hiveVKT::CVkContext::getInstance()->createContext();

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_pWindow = CREATE_WINDOW;

	EXPECT_FALSE(m_Swapchain.createSwapchain(m_pWindow, m_SupportedImageUsages));
	
	m_Swapchain.destroySwapchain();
	hiveVKT::CVkContext::getInstance()->destroyContext();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

//���Ե㣺�������Ѿ�����vulkan�����ĺ󣬽���swap chain�����٣�����ɰ�Ԥ�ڴ���
//
TEST_F(Test_VkSwapchain, DestroyAfterDestructContext)
{
	hiveVKT::CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	hiveVKT::CVkContext::getInstance()->setEnablePresentationHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableDebugUtilsHint(true);
	hiveVKT::CVkContext::getInstance()->createContext();

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_pWindow = CREATE_WINDOW;

	EXPECT_TRUE(m_Swapchain.createSwapchain(m_pWindow, m_SupportedImageUsages));
	
	hiveVKT::CVkContext::getInstance()->destroyContext();
	m_Swapchain.destroySwapchain();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

//���Ե㣺ʹ��glfwʱ����Ĭ�ϴ���OpenGL�����ġ�����ͨ����glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API)��ֹ��
//		 ���������������ԣ��ڴ��������������£����������ȷ�����ڴ�����£�glfwCreateWindowSurface�ڲ��������
//		 vkCreateWin32SurfaceKHR
TEST_F(Test_VkSwapchain, CreateUnderOpenGLContext)
{
	hiveVKT::CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
	hiveVKT::CVkContext::getInstance()->setEnablePresentationHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableDebugUtilsHint(true);
	hiveVKT::CVkContext::getInstance()->setEnableApiDumpHint(true);
	hiveVKT::CVkContext::getInstance()->createContext();

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_pWindow = CREATE_WINDOW;

	EXPECT_FALSE(m_Swapchain.createSwapchain(m_pWindow, m_SupportedImageUsages));

	m_Swapchain.destroySwapchain();
	hiveVKT::CVkContext::getInstance()->destroyContext();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();

	hiveVKT::CVkCallParser Parser;
	EXPECT_TRUE(Parser.parse("vk_apidump.txt"));

	auto CreateSwapChainCallInfo = Parser.getVkCallInfoByFunctionName(0, 0, "vkCreateWin32SurfaceKHR");
	EXPECT_EQ(CreateSwapChainCallInfo.size(), 0);
}
