#include "pch.h"
#include "VkInstanceCreator.hpp"
#include "VkDebugMessenger.hpp"

using namespace hiveVKT;

class Test_CreateVkInstance : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		glfwInit(); //HACK: glfwInit should not be called inside this test class.
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(m_DebugMessenger.destroyDebugMessenger(m_VkInstance));
		ASSERT_NO_THROW(m_VkInstance.destroy());
	}

	vk::Instance m_VkInstance;
	CVkDebugMessenger m_DebugMessenger;
};

TEST_F(Test_CreateVkInstance, CreateVkInstance)
{
	CVkInstanceCreator InstanceCreator;
	ASSERT_NO_THROW((m_VkInstance = InstanceCreator.create())); //NOTE: if the creation of vk::Instance failed, an exception should be thrown.
	ASSERT_NO_THROW(m_DebugMessenger.setupDebugMessenger(m_VkInstance));
	ASSERT_EQ(m_DebugMessenger.getWarningAndErrorCount(), 0);
}