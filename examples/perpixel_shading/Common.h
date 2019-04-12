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
	};

	struct SUniformBufferObject
	{
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Projection;
	};
}