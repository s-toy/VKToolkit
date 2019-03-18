#pragma once
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>
#include <GLM/glm.hpp>
#include <array>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace DeferredShading
{
#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 600

#define PER_VERTEX_DATA_BINDING 0
#define PER_INSTANCE_DATA_BINDING 1

#define MAX_FRAMES_IN_FLIGHT 2

	struct SVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription VertexInputBindingDescription = {};
			VertexInputBindingDescription.binding = PER_VERTEX_DATA_BINDING;
			VertexInputBindingDescription.stride = sizeof(SVertex);
			VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return VertexInputBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescriptions = {};
			VertexInputAttributeDescriptions[0].binding = PER_VERTEX_DATA_BINDING;
			VertexInputAttributeDescriptions[0].location = 0;
			VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			VertexInputAttributeDescriptions[0].offset = offsetof(SVertex, Position);
			VertexInputAttributeDescriptions[1].binding = PER_VERTEX_DATA_BINDING;
			VertexInputAttributeDescriptions[1].location = 1;
			VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			VertexInputAttributeDescriptions[1].offset = offsetof(SVertex, TexCoord);
			VertexInputAttributeDescriptions[2].binding = PER_VERTEX_DATA_BINDING;
			VertexInputAttributeDescriptions[2].location = 2;
			VertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
			VertexInputAttributeDescriptions[2].offset = offsetof(SVertex, Normal);

			return VertexInputAttributeDescriptions;
		}

		bool operator ==(const SVertex& vOtherVertex)const
		{
			return (Position == vOtherVertex.Position) && (TexCoord == vOtherVertex.TexCoord) && (Normal == vOtherVertex.Normal);
		}
	};

	struct SInstanceData
	{
		glm::vec3 Position;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription VertexInputBindingDescription = {};
			VertexInputBindingDescription.binding = PER_INSTANCE_DATA_BINDING;
			VertexInputBindingDescription.stride = sizeof(SInstanceData);
			VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

			return VertexInputBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 1> VertexInputAttributeDescriptions = {};
			VertexInputAttributeDescriptions[0].binding = PER_INSTANCE_DATA_BINDING;
			VertexInputAttributeDescriptions[0].location = 3;
			VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			VertexInputAttributeDescriptions[0].offset = offsetof(SInstanceData, Position);

			return VertexInputAttributeDescriptions;
		}
	};

	const std::vector<SInstanceData> gInstanceData4Model = {
		{glm::vec3(-4.5f,0.0f,0.0f)},
		{glm::vec3(-1.5f,0.0f,0.0f)},
		{glm::vec3(1.5f,0.0f,0.0f)},
		{glm::vec3(4.5f,0.0f,0.0f)}
	};

	struct SQuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription VertexInputBindingDescription = {};
			VertexInputBindingDescription.binding = 0;
			VertexInputBindingDescription.stride = sizeof(SQuadVertex);
			VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return VertexInputBindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 2> VertexInputAttributeDescriptions = {};
			VertexInputAttributeDescriptions[0].binding = 0;
			VertexInputAttributeDescriptions[0].location = 0;
			VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			VertexInputAttributeDescriptions[0].offset = offsetof(SQuadVertex, Position);
			VertexInputAttributeDescriptions[1].binding = 0;
			VertexInputAttributeDescriptions[1].location = 1;
			VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			VertexInputAttributeDescriptions[1].offset = offsetof(SQuadVertex, TexCoord);

			return VertexInputAttributeDescriptions;
		}
	};

	const std::vector<SQuadVertex> gQuadVertexData = {
		{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
		{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
		{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint32_t> gQuadIndexData = { 0,2,1,0,3,2 };

	const std::vector<VkFormat> gDepthFormatSet = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	struct SUniformBufferObject_OffScreen
	{
		glm::mat4 Model = glm::mat4();
		glm::mat4 View = glm::mat4();
		glm::mat4 Projection = glm::mat4();
	};

	struct SUniformBufferObject_Deferred
	{
		glm::vec3 ViewPosition = glm::vec3();
	};

	std::vector<char> ReadFile(const std::string& vFileName);
}

namespace std
{
	template<> struct hash<DeferredShading::SVertex>
	{
		size_t operator()(DeferredShading::SVertex const& vVertex)const
		{
			return (hash<glm::vec3>()(vVertex.Position) << 1) ^ (hash<glm::vec2>()(vVertex.TexCoord) << 1) ^ (hash<glm::vec3>()(vVertex.Normal) << 1);
		}
	};
}