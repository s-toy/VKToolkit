#include "pch.h"
#include "GLFW/glfw3.h"
#include "VkContext.h"
#include "VkSwapchain.h"

//���Ե㣺���Դ���մ���ָ��
//
TEST(Test_VkSwapchain, CreateSwapchainWithNullWindow)
{

}

//���Ե㣺���Դ��������Ĵ���ָ�룬�������Ĳ������Ϸ�
//
TEST(Test_VkSwapchain, CreateSwapchainWithValidWindow)
{

}

//���Ե㣺�����ؽ�swap chain
//
TEST(Test_VkSwapchain, RecreateSwapchain)
{

}

//���Ե㣺����ʹ�÷Ƿ���image usage flags����ʱ�����������������
//
TEST(Test_VkSwapchain, CreateWithInvalidImageUsageFlags)
{

}

//���Ե㣺�����ظ�����create���������������������
//
TEST(Test_VkSwapchain, DuplicateCreationCall)
{

}

//���Ե㣺������δ��ʼ��Context֮ǰ����swap chainʱ�����������������
//
TEST(Test_VkSwapchain, CreateBeforeInitializeContext)
{

}

//���Ե㣺�����ڲ����ʵ��������д���swap chain��������԰�Ԥ�ڴ���
//
TEST(Test_VkSwapchain, CreateWithUnsuitableContext)
{

}

//���Ե㣺�������Ѿ�����vulkan�����ĺ󣬽���swap chain�����٣�����ɰ�Ԥ�ڴ���
//
TEST(Test_VkSwapchain, DestroyAfterDestructContext)
{

}

//���Ե㣺ʹ��glfwʱ����Ĭ�ϴ���OpenGL�����ġ�����ͨ����glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API)��ֹ��
//		 ���������������ԣ��ڴ��������������£����������ȷ����
//
TEST(Test_VkSwapchain, CreateUnderOpenGLContext)
{

}
