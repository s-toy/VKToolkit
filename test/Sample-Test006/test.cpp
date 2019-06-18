#include "pch.h"
#include "VkContext.h"
#include "VKUtility.h"
#include <Windows.h>

using namespace hiveVKT;

class CreateBufferTest : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		hiveVKT::CVkContext::getInstance()->setExtraFuncStatus(PREFER_DISCRETE_GPU | ENABLE_DEBUG_UTILS);
		ASSERT_NO_THROW(CVkContext::getInstance()->createContext());
	}

	virtual void TearDown() override
	{
		ASSERT_NO_THROW(CVkContext::getInstance()->destroyContext());
	}

	std::vector<std::pair<vk::BufferUsageFlags, vk::MemoryPropertyFlags>> m_BufferInfos =
	{
		{vk::BufferUsageFlagBits::eVertexBuffer,	vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent},		// vertex buffer without staging buffer
		{vk::BufferUsageFlagBits::eIndexBuffer,		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent},		// index buffer without staging buffer
		{vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,	vk::MemoryPropertyFlagBits::eDeviceLocal},			// vertex buffer with staging buffer
		{vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,	vk::MemoryPropertyFlagBits::eDeviceLocal},				// index buffer with staging buffer
		{vk::BufferUsageFlagBits::eUniformBuffer,	vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent},		// uniform buffer
		{vk::BufferUsageFlagBits::eTransferSrc,		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent},		// staging buffer
	};
};

class CreateBufferDeathTest : public CreateBufferTest 
{
protected:
	vk::Buffer			m_Buffer;
	vk::DeviceMemory	m_BufferMemory;
};

//���Ե㣺��������������������ȷ����Buffer
//
TEST_F(CreateBufferTest, CreateVkBuffer) 
{
	int BufferSize = 100;
	
	std::vector<vk::Buffer>			Buffers;
	std::vector<vk::DeviceMemory>	BufferMemories;

	size_t NumBuffersToCreate = m_BufferInfos.size();

	Buffers.resize(NumBuffersToCreate);
	BufferMemories.resize(NumBuffersToCreate);

	vk::Result Result;

	for (size_t i = 0; i < NumBuffersToCreate; i++)
	{
		vk::Buffer		 Buffer;
		vk::DeviceMemory BufferMemory;

		Result = createBuffer(BufferSize, m_BufferInfos[i].first, m_BufferInfos[i].second, Buffers[i], BufferMemories[i]);
		
		EXPECT_EQ(vk::Result::eSuccess, Result);
	}
}

//���Ե㣺����Ƿ���BufferSizeʱ������Ӧ���׳��쳣
//
TEST_F(CreateBufferDeathTest, CreateWithInvalidBufferSize)
{
	MEMORYSTATUSEX SystemMemoryStatus;
	vk::DeviceSize BufferSize = SystemMemoryStatus.ullTotalPhys + 100;
	
	EXPECT_EXIT(createBuffer(BufferSize, m_BufferInfos[0].first, m_BufferInfos[0].second, m_Buffer, m_BufferMemory),
		testing::ExitedWithCode(static_cast<int>(vk::Result::eErrorOutOfDeviceMemory)),  // to be modified according to new result format
		"");
}

//���Ե㣺����Ƿ���BufferUsageʱ������Ӧ���׳��쳣
//
TEST_F(CreateBufferDeathTest, CreateWithInvalidBufferUsage)
{
	vk::DeviceSize			BufferSize = 100;
	vk::BufferUsageFlags	InvalidBufferUsage = static_cast<vk::BufferUsageFlagBits>(0x00008000);
	
	EXPECT_EXIT(createBuffer(BufferSize, InvalidBufferUsage, m_BufferInfos[0].second, m_Buffer, m_BufferMemory),
		testing::ExitedWithCode(static_cast<int>(vk::Result::eNotReady)),	// to be modified according to new result format
		"");
}

//���Ե㣺����Ƿ���MemoryPropertyʱ������Ӧ���׳��쳣
//
TEST_F(CreateBufferDeathTest, CreateWithInvalidMemoryProperty)
{
	vk::DeviceSize				BufferSize = 100;
	vk::MemoryPropertyFlags		InvalidMemoryProperty = static_cast<vk::MemoryPropertyFlagBits>(0x00008000);

	EXPECT_EXIT(createBuffer(BufferSize, m_BufferInfos[0].first, InvalidMemoryProperty, m_Buffer, m_BufferMemory),
		testing::ExitedWithCode(static_cast<int>(vk::Result::eNotReady)),	// to be modified according to new result format
		"");
}

//���Ե㣺��δ��ʼ��Contextʱ����buffer�����������������
//
TEST(CreateBufferTest, CreateBeforeInitializeContext)
{
	vk::DeviceSize				BufferSize = 100;
	vk::BufferUsageFlags		BufferUsage = vk::BufferUsageFlagBits::eVertexBuffer;
	vk::MemoryPropertyFlags		MemoryProperty = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

	vk::Buffer			Buffer;
	vk::DeviceMemory	BufferMemory;

	vk::Result Result;
	Result = createBuffer(BufferSize, BufferUsage, MemoryProperty, Buffer, BufferMemory);

	EXPECT_EQ(vk::Result::eSuccess, Result);	// not sure about the result yet
}