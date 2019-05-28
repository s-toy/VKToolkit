#include "Mesh.h"

//***********************************************************************************************
//FUNCTION:
hiveVKT::CMesh::CMesh(const std::vector<float>& vVertexData, const std::vector<uint32_t>& vIndexData, vk::DescriptorSet vTextureDescriptorSet)
{
	m_VertexData = vVertexData;
	m_IndexData = vIndexData;
	m_pTextureDescriptorSet = vTextureDescriptorSet;

	__generateBufferResource(vVertexData, vk::BufferUsageFlagBits::eVertexBuffer, m_pVertexBuffer, m_pVertexBufferDeviceMemory);
	__generateBufferResource(vIndexData, vk::BufferUsageFlagBits::eIndexBuffer, m_pIndexBuffer, m_pIndexBufferDeviceMemory);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CMesh::draw(const vk::CommandBuffer vCommandBuffer, const vk::PipelineLayout vPipelineLayout, const std::vector<vk::DescriptorSet>& vOtherDescriptorSet2BeBound)
{
	_ASSERT(vCommandBuffer && vPipelineLayout);

	vk::Buffer VertexBuffers[] = { m_pVertexBuffer };
	vk::DeviceSize Offsets[] = { 0 };

	std::vector<vk::DescriptorSet> DescriptorSet2BeBound(vOtherDescriptorSet2BeBound);
	DescriptorSet2BeBound.emplace_back(m_pTextureDescriptorSet);

	vCommandBuffer.bindVertexBuffers(0, 1, VertexBuffers, Offsets);
	vCommandBuffer.bindIndexBuffer(m_pIndexBuffer, 0, vk::IndexType::eUint32);
	vCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vPipelineLayout, 0, DescriptorSet2BeBound, nullptr);
	vCommandBuffer.drawIndexed(static_cast<uint32_t>(m_IndexData.size()), 1, 0, 0, 0);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CMesh::destroy(vk::Device vDevice)
{
	vDevice.destroyBuffer(m_pVertexBuffer);
	vDevice.freeMemory(m_pVertexBufferDeviceMemory);
	vDevice.destroyBuffer(m_pIndexBuffer);
	vDevice.freeMemory(m_pIndexBufferDeviceMemory);
}