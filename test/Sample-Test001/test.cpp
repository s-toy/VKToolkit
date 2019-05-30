#include "pch.h"
#include "VkContext.h"

using namespace hiveVKT;

class Test_VkContext : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
	}

	virtual void TearDown() override
	{
	}
};

//测试点：创建默认VkContext
TEST_F(Test_VkContext, CreateContext)
{

}

//测试点：显式析构VkContext
TEST_F(Test_VkContext, DestroyContext)
{

}

//测试点：VkContext创建后不能动态更改
TEST_F(Test_VkContext, ForBidModificationAfterContextCreated)
{

}

//测试点: 设置倾向选择的CPU类型,集显或者独显
TEST_F(Test_VkContext, SetPreferGPU)
{

}

//测试点：要求选择的GPU必须支持图形功能
TEST_F(Test_VkContext, ForceGraphicsFunction)
{

}

//测试点：要求选择的GPU必须支持计算功能
TEST_F(Test_VkContext, ForceComputeFunction)
{

}

//测试点：要求选择的GPU必须支持传输功能
TEST_F(Test_VkContext, ForceTransferFunction)
{

}

//测试点：开启验证层的Debug功能
TEST_F(Test_VkContext, EnableDebugUtils)
{

}

//测试点：开启Presentation支持
TEST_F(Test_VkContext, EnablePresentation)
{

}

//测试点：开启Api调用记录支持
TEST_F(Test_VkContext, EnableApiDump)
{

}

//测试点：开启帧率记录支持
TEST_F(Test_VkContext, EnableFpsMonitor)
{

}

//测试点：开启屏幕截图支持？？需要一堆功能去支持显示
TEST_F(Test_VkContext, EnableScreenshot)
{

}

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