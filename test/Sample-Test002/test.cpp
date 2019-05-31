#include "pch.h"
#include "GLFW/glfw3.h"
#include "VkContext.h"
#include "VkSwapchain.h"

//测试点：测试传入空窗口指针
//
TEST(Test_VkSwapchain, CreateSwapchainWithNullWindow)
{

}

//测试点：测试传入正常的窗口指针，且其他的参数都合法
//
TEST(Test_VkSwapchain, CreateSwapchainWithValidWindow)
{

}

//测试点：测试重建swap chain
//
TEST(Test_VkSwapchain, RecreateSwapchain)
{

}

//测试点：测试使用非法的image usage flags创建时，程序可以正常处理
//
TEST(Test_VkSwapchain, CreateWithInvalidImageUsageFlags)
{

}

//测试点：测试重复调用create方法，程序可以正常处理
//
TEST(Test_VkSwapchain, DuplicateCreationCall)
{

}

//测试点：测试在未初始化Context之前创建swap chain时，程序可以正常工作
//
TEST(Test_VkSwapchain, CreateBeforeInitializeContext)
{

}

//测试点：测试在不合适的上下文中创建swap chain，程序可以按预期处理
//
TEST(Test_VkSwapchain, CreateWithUnsuitableContext)
{

}

//测试点：测试在已经销毁vulkan上下文后，进行swap chain的销毁，程序可按预期处理
//
TEST(Test_VkSwapchain, DestroyAfterDestructContext)
{

}

//测试点：使用glfw时，会默认创建OpenGL上下文【可以通过：glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API)禁止】
//		 本测试用例将测试，在此情况发生的情况下，程序可以正确处理
//
TEST(Test_VkSwapchain, CreateUnderOpenGLContext)
{

}
