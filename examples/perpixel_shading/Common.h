#pragma once
#include <vector>
#include <optional>
#include <array>
#include <vulkan/vulkan.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/glm.hpp>
#include <GLM/gtx/hash.hpp>

namespace VulkanApp
{
#define MAX_FRAMES_IN_FLIGHT 2

#if defined(DEBUG) | defined(_DEBUG)
	const bool g_enableValidationLayers = true;
#else
	const bool g_enableValidationLayers = false;
#endif

	struct SVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;

		bool operator ==(const SVertex& vOtherVertex)const
		{
			return (Position == vOtherVertex.Position) && (Normal == vOtherVertex.Normal) && (TexCoord == vOtherVertex.TexCoord);
		}
	};

	struct SUniformBufferObject
	{
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Projection;
	};
}

namespace std
{
	template<> struct hash<VulkanApp::SVertex>
	{
		size_t operator()(VulkanApp::SVertex const& vVertex)const
		{
			return ((hash<glm::vec3>()(vVertex.Position) ^ (hash<glm::vec3>()(vVertex.Normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vVertex.TexCoord) << 1);
		}
	};
}