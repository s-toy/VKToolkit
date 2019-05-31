#include "pch.h"
#include "VkContext.h"
#include "VKUtility.h"

using namespace hiveVKT;

class Test_CreateBuffer : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}
};


//测试点：传入正常参数，可以正确生成Buffer
//
TEST_F(Test_CreateBuffer, CreateVkBuffer) 
{

}

//测试点：传入非法的BufferSize时，程序应该抛出异常
//
TEST_F(Test_CreateBuffer, CreateWithInvalidBufferSize)
{

}

//测试点：传入非法的BufferUsage时，程序应该抛出异常
//
TEST_F(Test_CreateBuffer, CreateWithInvalidBufferUsage) 
{

}

//测试点：传入非法的MemoryProperty时，程序应该抛出异常
//
TEST_F(Test_CreateBuffer, CreateWithInvalidMemoryProperty)
{

}

//测试点：在未初始化Context时创建buffer，程序可以正常工作
//
TEST_F(Test_CreateBuffer, CreateBeforeInitializeContext) 
{

}