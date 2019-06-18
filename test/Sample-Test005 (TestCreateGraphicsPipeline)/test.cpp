#include "pch.h"
#include "VkDebugMessenger.h"
#include "VkGraphicsPipelineCreator.h"
#include "VkContext.h"
#include "VkRenderPassCreator.h"
#include "VkShaderModuleCreator.h"

using namespace hiveVKT;
const std::string vertFile = "C:/Users/ACE/source/vk_toolkit/test/Sample-Test005 (TestCreateGraphicsPipeline)/vert.spv";
const std::string fragFile = "C:/Users/ACE/source/vk_toolkit/test/Sample-Test005 (TestCreateGraphicsPipeline)/frag.spv";

class Test_CreateGraphicsPipeline : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		hiveVKT::CVkContext::getInstance()->enableContextFeature(PREFER_DISCRETE_GPU | ENABLE_DEBUG_UTILS);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());
		m_Device = CVkContext::getInstance()->getVulkanDevice();
		ASSERT_TRUE(m_Device);
		/*m_DescriptorSetLayout = _createCorrectDescriptorSetLayout();
		ASSERT_TRUE(m_DescriptorSetLayout);*/
	
		m_TestExtent = { 800,600 };
		__addViewPortAndScissor();
		__addColorBlendAttachmentState();
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	CVkDebugUtilsMessenger m_DebugMessenger;
	vk::Extent2D m_TestExtent;
	CVkGraphicsPipelineCreator m_GraphicsPipelineCreator;
	vk::Pipeline m_GraphicsPipeline;
	vk::Device m_Device;
	vk::PipelineLayout m_PipelineLayout;
	vk::PipelineCache m_PipelineCache = nullptr;
	vk::RenderPass m_RenderPass;
	uint32_t m_SubPassIdx;
	//为了创建pipelineLayout需要描述符布局
	vk::DescriptorSetLayout m_DescriptorSetLayout;
	
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
	vk::RenderPass _createRenderPassWithMsaa()
	{
		vk::RenderPass renderPass;
		CVkRenderPassCreator renderpassCreator;
		renderpassCreator.addAttachment({ vk::Format::eB8G8R8A8Unorm }, vk::ImageLayout::eColorAttachmentOptimal);//使用多重采样
		renderpassCreator.addAttachment({ vk::Format::eB8G8R8A8Unorm }, vk::ImageLayout::ePresentSrcKHR);//使用多重采样
		SSubPassDescription SubpassDesc;
		SubpassDesc.ColorAttachmentSet = { vk::AttachmentReference {0, vk::ImageLayout::eColorAttachmentOptimal} };
		SubpassDesc.ResolveAttachment = { vk::AttachmentReference {1, vk::ImageLayout::eColorAttachmentOptimal} };
		renderpassCreator.addSubpass(SubpassDesc);
		EResult r = renderpassCreator.create(m_Device, renderPass);
		EXPECT_EQ(r, EResult::eSuccess);
		return renderPass;
	}
	vk::RenderPass _createRuntimeErrorRenderPass()
	{
		return vk::RenderPass();
	}
	uint32_t _createWrongSubPass()
	{
		return 1u;
	}

	uint32_t __createCorrectSubpass()
	{
		return 0u;
	}

	vk::PipelineLayout _createCorrectPipelineLayout()
	{
		vk::PipelineLayout pipelineLayout;
		//没有描述符布局的pipelineLayout
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = { vk::PipelineLayoutCreateFlags(), 0U,(const vk::DescriptorSetLayout*)nullptr,
		0U,(const vk::PushConstantRange*)nullptr };
		pipelineLayout = m_Device.createPipelineLayout(pipelineLayoutCreateInfo);
		return pipelineLayout;

	}
	vk::DescriptorSetLayout _createCorrectDescriptorSetLayout()
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

	vk::RenderPass _createCorrectRenderPass()
	{
		vk::RenderPass renderPass;
		CVkRenderPassCreator renderpassCreator;
		renderpassCreator.addAttachment({ vk::Format::eB8G8R8A8Unorm }, vk::ImageLayout::ePresentSrcKHR);//不使用多重采样
		SSubPassDescription SubpassDesc;
		SubpassDesc.ColorAttachmentSet = { vk::AttachmentReference {0, vk::ImageLayout::eColorAttachmentOptimal} };
		renderpassCreator.addSubpass(SubpassDesc);
		EResult r = renderpassCreator.create(m_Device, renderPass);
		EXPECT_EQ(r, EResult::eSuccess);
		return renderPass;
	}

	void _addCorrectShaderStage()
	{
		hiveVKT::CVkShaderModuleCreator ShaderModuleCreator;
		vk::ShaderModule vertexShaderModule;
		ShaderModuleCreator.create(vertFile, vertexShaderModule);// createUnique(vertFile);出了这个作用域就会销毁module
		vk::ShaderModule fragmentShaderModule;
		ShaderModuleCreator.create(fragFile, fragmentShaderModule);
		vk::PipelineShaderStageCreateInfo vertShader({}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main");
		vk::PipelineShaderStageCreateInfo fragShader({}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main");
		m_GraphicsPipelineCreator.addShaderStage(vertShader);
		m_GraphicsPipelineCreator.addShaderStage(fragShader);
	}

	void _addOnlyVertexShaderStage()
	{
		hiveVKT::CVkShaderModuleCreator ShaderModuleCreator;
		vk::ShaderModule vertexShaderModule;
		ShaderModuleCreator.create(vertFile, vertexShaderModule);
		vk::PipelineShaderStageCreateInfo vertShader({}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main");
		m_GraphicsPipelineCreator.addShaderStage(vertShader);
	}

	void _addTwoSameVertexShaderStage()
	{
		hiveVKT::CVkShaderModuleCreator ShaderModuleCreator;
		vk::ShaderModule vertexShaderModule;
		ShaderModuleCreator.create(vertFile, vertexShaderModule);
		vk::PipelineShaderStageCreateInfo vertShader({}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main");
		m_GraphicsPipelineCreator.addShaderStage(vertShader);
		m_GraphicsPipelineCreator.addShaderStage(vertShader);
	}
private:

	void __addViewPortAndScissor()
	{
		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_TestExtent.width;
		viewport.height = (float)m_TestExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		m_GraphicsPipelineCreator.addViewport(viewport);
		vk::Rect2D scissor = {};
		scissor.offset = { 0,0 };
		scissor.extent = m_TestExtent;
		m_GraphicsPipelineCreator.addScissor(scissor);
	}

	void __addColorBlendAttachmentState()
	{
		vk::PipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
		ColorBlendAttachmentState.blendEnable = false;
		ColorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		//颜色混合直接相加
		ColorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
		ColorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;
		ColorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eOne;
		ColorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eZero;
		ColorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		ColorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		m_GraphicsPipelineCreator.addColorBlendAttachment(ColorBlendAttachmentState);
	}

	

};
//***********************************************************************************************
//测试点: 成功创建一个不带描述符布局的Pipeline
TEST_F(Test_CreateGraphicsPipeline, createSuccessPipeline)
{
	_addCorrectShaderStage();
	m_PipelineLayout = _createCorrectPipelineLayout();
	ASSERT_TRUE(m_PipelineLayout);
	m_SubPassIdx = __createCorrectSubpass();
	m_RenderPass = _createCorrectRenderPass();
	ASSERT_TRUE(m_RenderPass);
	EResult r = m_GraphicsPipelineCreator.create(m_GraphicsPipeline, m_Device, m_PipelineLayout,
		nullptr, m_RenderPass, m_SubPassIdx);
	EXPECT_EQ(r, EResult::eSuccess);
	ASSERT_EQ(0, m_DebugMessenger.getWarningCount() + m_DebugMessenger.getWarningCount());
}

//***********************************************************************************************
//测试点: 成功创建一个拥有多重采样的renderpass的Pipeline
TEST_F(Test_CreateGraphicsPipeline, createSuccessPipelineWithMsaaRenderPass)
{
	_addCorrectShaderStage();
	m_PipelineLayout = _createCorrectPipelineLayout();
	ASSERT_TRUE(m_PipelineLayout);
	m_SubPassIdx = __createCorrectSubpass();
	m_RenderPass = _createRenderPassWithMsaa();
	ASSERT_TRUE(m_RenderPass);
	EResult r = m_GraphicsPipelineCreator.create(m_GraphicsPipeline, m_Device, m_PipelineLayout,
		nullptr, m_RenderPass, m_SubPassIdx);
	EXPECT_EQ(r, EResult::eSuccess);
	ASSERT_EQ(0, m_DebugMessenger.getWarningCount() + m_DebugMessenger.getWarningCount());
}



//***********************************************************************************************
//测试点: 创建一个只有一个shaderstage的Pipeline
TEST_F(Test_CreateGraphicsPipeline, createFailurePipelineWithOneShaderSatge)
{
	_addOnlyVertexShaderStage();
	m_PipelineLayout = _createCorrectPipelineLayout();
	ASSERT_TRUE(m_PipelineLayout);
	m_SubPassIdx = __createCorrectSubpass();
	m_RenderPass = _createCorrectRenderPass();
	ASSERT_TRUE(m_RenderPass);
	EResult r = m_GraphicsPipelineCreator.create(m_GraphicsPipeline, m_Device, m_PipelineLayout,
		nullptr, m_RenderPass, m_SubPassIdx);
	EXPECT_EQ(r, EResult::eErrorInitializationFailed);
}

//***********************************************************************************************
//测试点: 创建一个含有两个相同的vertex shader的Pipeline
TEST_F(Test_CreateGraphicsPipeline, createFailurePipelineWithTwoSameShaderSatge)
{
	_addTwoSameVertexShaderStage();
	m_PipelineLayout = _createCorrectPipelineLayout();
	ASSERT_TRUE(m_PipelineLayout);
	m_SubPassIdx = __createCorrectSubpass();
	m_RenderPass = _createCorrectRenderPass();
	ASSERT_TRUE(m_RenderPass);
	//TODO 测试用例不能通过，因为取不到shaderStageSet里面的详细信息，比如0号元素的module阶段
	EResult r = m_GraphicsPipelineCreator.create(m_GraphicsPipeline, m_Device, m_PipelineLayout,
		nullptr, m_RenderPass, m_SubPassIdx);
	EXPECT_EQ(r, EResult::eErrorInitializationFailed);
}

TEST_F(Test_CreateGraphicsPipeline, createFailurePipelineWithWrongPipelineLayout)
{
	m_PipelineLayout = _createWrongPipelineLayout();
	//TODO 捕获弹出的异常，异常类型为logic error并且异常语句为pipelineLayout is wrong
	m_PipelineLayout = _createRuntimeErrorPipelineLayout();
	//TODO 捕获弹出的异常，异常类型为runtime error并且异常语句为pipelineLayout is runtime wrong
}


//***********************************************************************************************
//测试点: 创建一个拥有大于了renderPass中的subapss的subpassIdx的Pipeline
TEST_F(Test_CreateGraphicsPipeline, createFailurePipelineWithWrongSubPass)
{
	_addCorrectShaderStage();
	m_PipelineLayout = _createCorrectPipelineLayout();
	ASSERT_TRUE(m_PipelineLayout);
	m_SubPassIdx = _createWrongSubPass();
	m_RenderPass = _createCorrectRenderPass();
	ASSERT_TRUE(m_RenderPass);
	//TODO 底层的报错函数，我取不到renderPass中的subpass的数量
	EResult r = m_GraphicsPipelineCreator.create(m_GraphicsPipeline, m_Device, m_PipelineLayout,
		nullptr, m_RenderPass, m_SubPassIdx);
	EXPECT_EQ(r, EResult::eErrorInvalidParameters);
	ASSERT_EQ(0, m_DebugMessenger.getWarningCount() + m_DebugMessenger.getWarningCount());
}

