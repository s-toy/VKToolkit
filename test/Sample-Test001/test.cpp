#include "pch.h"
#include "VkContext.h"
#include "VkDebugMessenger.h"

using namespace hiveVKT;

class Test_CreateVkInstance : public ::testing::Test
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

	CVkDebugUtilsMessenger m_DebugMessenger;
};

TEST_F(Test_CreateVkInstance, CreateVkInstance)
{
	ASSERT_NO_THROW(m_DebugMessenger.setupDebugUtilsMessenger());
	ASSERT_EQ(m_DebugMessenger.getWarningAndErrorCount(), 0);
	ASSERT_NO_THROW(m_DebugMessenger.destroyDebugUtilsMessenger());
}