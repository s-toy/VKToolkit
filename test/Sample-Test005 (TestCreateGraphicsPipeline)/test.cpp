#include "pch.h"
#include "VkDebugMessenger.h"
#include "VkGraphicsPipelineCreator.h"
#include "VkContext.h"
#include "VkRenderPassCreator.h"

using namespace hiveVKT;

class Test_CreateGraphicsPipeline : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		CVkContext::getInstance()->setPreferDiscreteGpuHint(true);
		CVkContext::getInstance()->setEnableDebugUtilsHint(true);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());
		//Device创建失败了。。。所以不能实现，只能写接口了
		m_Device = CVkContext::getInstance()->getVulkanDevice();
		ASSERT_TRUE(m_Device);
		m_DescriptorSetLayout = __createCorrectDescriptorSetLayout();
		ASSERT_TRUE(m_DescriptorSetLayout);
		m_PipelineLayout = __createCorrectPipelineLayout();
		ASSERT_TRUE(m_PipelineLayout);
		m_RenderPass = __createCorrectRenderPass();
		ASSERT_TRUE(m_RenderPass);
		m_SubPass = __createCorrectSubpass();;
		ASSERT_TRUE(m_SubPass);
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	CVkDebugUtilsMessenger m_DebugMessenger;
	CVkGraphicsPipelineCreator m_GraphicsPipelineCreator;
	vk::Pipeline m_GraphicsPipeline;
	vk::Device m_Device;
	vk::PipelineLayout m_PipelineLayout;
	vk::PipelineCache m_PipelineCache = nullptr;
	vk::RenderPass m_RenderPass;
	uint32_t m_SubPass;
	//为了创建pipelineLayout需要描述符布局
	vk::DescriptorSetLayout m_DescriptorSetLayout;
	
	vk::Device _createWrongDevice()//单独都是错误的
	{
		return vk::Device();
	}
	vk::Device _createRuntimeErrorDevice()//单独看是正常的，但是与上下文不配合的逻辑设备
	{
		return vk::Device();
	}
	vk::PipelineLayout _createWrongPipelineLayout()
	{
		return vk::PipelineLayout();
	}
	vk::PipelineLayout _createRuntimeErrorPipelineLayout()
	{
		return vk::PipelineLayout();
	}
	vk::RenderPass _createWrongRenderPass()
	{
		return vk::RenderPass();
	}
	vk::RenderPass _createRuntimeErrorRenderPass()
	{
		return vk::RenderPass();
	}
	uint32_t _createWrongSubPass()
	{
		return uint32_t();
	}
	uint32_t _createRuntimeErrorSubPass()
	{
		return uint32_t();
	}
private:

	uint32_t __createCorrectSubpass()
	{
		//TODO
	}

	vk::PipelineLayout __createCorrectPipelineLayout()
	{
		vk::PipelineLayout pipelineLayout;
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = { vk::PipelineLayoutCreateFlags(), 1U,(const vk::DescriptorSetLayout*)&m_DescriptorSetLayout,
		0U,(const vk::PushConstantRange*)nullptr };
		pipelineLayout = m_Device.createPipelineLayout(pipelineLayoutCreateInfo);
		return pipelineLayout;

	}
	vk::DescriptorSetLayout __createCorrectDescriptorSetLayout()
	{
		vk::DescriptorSetLayout DescriptorSetLayout;

		//uniform变量的布局
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;  
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };
		//描述符布局的信息
		vk::DescriptorSetLayoutCreateInfo layoutInfo = { vk::DescriptorSetLayoutCreateFlags(), static_cast<uint32_t>(bindings.size()),
		(const vk::DescriptorSetLayoutBinding*)bindings.data() };
		DescriptorSetLayout = m_Device.createDescriptorSetLayout(layoutInfo);
		return DescriptorSetLayout;
	}

	vk::RenderPass __createCorrectRenderPass()
	{
		vk::RenderPass renderPass;
		CVkRenderPassCreator renderpassCreator;
		renderpassCreator.addAttachment({ vk::Format::eB8G8R8A8Unorm }, vk::ImageLayout::ePresentSrcKHR);//不使用多重采样
		renderPass = renderpassCreator.create(m_Device);
		return renderPass;
	}


};

TEST_F(Test_CreateGraphicsPipeline, createSuccessPipeline)
{
	ASSERT_EQ(vk::Result::eSuccess, m_GraphicsPipelineCreator.create(m_GraphicsPipeline, m_Device, m_PipelineLayout,
		nullptr, m_RenderPass, m_SubPass));
	ASSERT_EQ(0, m_DebugMessenger.getWarningCount() + m_DebugMessenger.getWarningCount());
}

TEST_F(Test_CreateGraphicsPipeline, createFailurePipelineWithWrongDevice)
{
	m_Device = _createWrongDevice();
	//TODO 捕获弹出的异常，异常类型为logic error并且异常语句为device is wrong
	//ASSERT_THROW("device is wrong",);
	m_Device = _createRuntimeErrorDevice();
	//TODO 捕获弹出的异常，异常类型为runtime error并且异常语句为device is runtime wrong
}

TEST_F(Test_CreateGraphicsPipeline, createFailurePipelineWithWrongPipelineLayout)
{
	m_PipelineLayout = _createWrongPipelineLayout();
	//TODO 捕获弹出的异常，异常类型为logic error并且异常语句为pipelineLayout is wrong
	m_PipelineLayout = _createRuntimeErrorPipelineLayout();
	//TODO 捕获弹出的异常，异常类型为runtime error并且异常语句为pipelineLayout is runtime wrong
}

TEST_F(Test_CreateGraphicsPipeline, createFailurePipelineWithWrongRenderPass)
{
	m_RenderPass = _createWrongRenderPass();
	//TODO 捕获弹出的异常，异常类型为logic error并且异常语句为RenderPass is wrong
	m_RenderPass = _createRuntimeErrorRenderPass();
	//TODO 捕获弹出的异常，异常类型为runtime error并且异常语句为RenderPass is runtime wrong
}

TEST_F(Test_CreateGraphicsPipeline, createFailurePipelineWithWrongSubPass)
{
	m_SubPass = _createWrongSubPass();
	//TODO 捕获弹出的异常，异常类型为logic error并且异常语句为subpass is wrong
	m_SubPass = _createRuntimeErrorSubPass();
	//TODO 捕获弹出的异常，异常类型为runtime error并且异常语句为subpass is runtime wrong
}

