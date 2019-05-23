#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/glm.hpp>

namespace VulkanApp
{
#define MAX_FRAMES_IN_FLIGHT 2

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