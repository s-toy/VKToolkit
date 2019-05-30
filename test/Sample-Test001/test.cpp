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
};

TEST_F(Test_CreateVkInstance, CreateVkInstance)
{
	auto DebugUtilsMessenger = CVkContext::getInstance()->getDebugUtilsMessenger();

	ASSERT_EQ(DebugUtilsMessenger.getWarningAndErrorCount(), 0);
}