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

//���Ե㣺����Ĭ��VkContext
TEST_F(Test_VkContext, CreateContext)
{

}

//���Ե㣺��ʽ����VkContext
TEST_F(Test_VkContext, DestroyContext)
{

}

//���Ե㣺VkContext�������ܶ�̬����
TEST_F(Test_VkContext, ForBidModificationAfterContextCreated)
{

}

//���Ե�: ��������ѡ���CPU����,���Ի��߶���
TEST_F(Test_VkContext, SetPreferGPU)
{

}

//���Ե㣺Ҫ��ѡ���GPU����֧��ͼ�ι���
TEST_F(Test_VkContext, ForceGraphicsFunction)
{

}

//���Ե㣺Ҫ��ѡ���GPU����֧�ּ��㹦��
TEST_F(Test_VkContext, ForceComputeFunction)
{

}

//���Ե㣺Ҫ��ѡ���GPU����֧�ִ��书��
TEST_F(Test_VkContext, ForceTransferFunction)
{

}

//���Ե㣺������֤���Debug����
TEST_F(Test_VkContext, EnableDebugUtils)
{

}

//���Ե㣺����Presentation֧��
TEST_F(Test_VkContext, EnablePresentation)
{

}

//���Ե㣺����Api���ü�¼֧��
TEST_F(Test_VkContext, EnableApiDump)
{

}

//���Ե㣺����֡�ʼ�¼֧��
TEST_F(Test_VkContext, EnableFpsMonitor)
{

}

//���Ե㣺������Ļ��ͼ֧�֣�����Ҫһ�ѹ���ȥ֧����ʾ
TEST_F(Test_VkContext, EnableScreenshot)
{

}

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