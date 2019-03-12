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
#define WINDOW_WIDTH  1600
#define WINDOW_HEIGHT 900

#define MAX_FRAMES_IN_FLIGHT 2

#if defined(DEBUG) | defined(_DEBUG)
	const bool g_enableValidationLayers = true;
#else
	const bool g_enableValidationLayers = false;
#endif

	struct SQueueFamilyIndices
	{
		std::optional<uint32_t> QueueFamily;

		bool IsComplete() { return QueueFamily.has_value(); }
	};

	struct SSwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> SurfaceFormatSet;
		std::vector<VkPresentModeKHR> PresentModeSet;
	};

	struct SVertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec2 TexCoord;

		bool operator ==(const SVertex& vOtherVertex)const
		{
			return (Position == vOtherVertex.Position) && (Color == vOtherVertex.Color) && (TexCoord == vOtherVertex.TexCoord);
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
			return ((hash<glm::vec3>()(vVertex.Position) ^ (hash<glm::vec3>()(vVertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(vVertex.TexCoord) << 1);
		}
	};
}