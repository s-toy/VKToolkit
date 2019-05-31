#include "pch.h"
#include "VkContext.h"
#include "VkDebugMessenger.h"
#include "VkShaderModuleCreator.h"
#include <vulkan/vulkan.hpp>

using namespace hiveVKT;

class Test_CreateVkShaderModule : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());

		auto DebugUtilsMessenger = CVkContext::getInstance()->getDebugUtilsMessenger();

		hiveVKT::CVkShaderModuleCreator ShaderModuleCreator;
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}
};


TEST_F(Test_CreateVkShaderModule, CreateShaderModule)
{

}

TEST(Test_ShaderModuleCreator, FeedCreatorWithWrongFileName)
{

}

TEST(Test_ShaderModuleCreator, FeedCreatorWithWrongFormat)
{

}

TEST(Test_ShaderModuleCreator, FeedCreatorWithEmptyFile)
{

}

