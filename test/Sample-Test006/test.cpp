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


//���Ե㣺��������������������ȷ����Buffer
//
TEST_F(Test_CreateBuffer, CreateVkBuffer) 
{

}

//���Ե㣺����Ƿ���BufferSizeʱ������Ӧ���׳��쳣
//
TEST_F(Test_CreateBuffer, CreateWithInvalidBufferSize)
{

}

//���Ե㣺����Ƿ���BufferUsageʱ������Ӧ���׳��쳣
//
TEST_F(Test_CreateBuffer, CreateWithInvalidBufferUsage) 
{

}

//���Ե㣺����Ƿ���MemoryPropertyʱ������Ӧ���׳��쳣
//
TEST_F(Test_CreateBuffer, CreateWithInvalidMemoryProperty)
{

}

//���Ե㣺��δ��ʼ��Contextʱ����buffer�����������������
//
TEST_F(Test_CreateBuffer, CreateBeforeInitializeContext) 
{

}