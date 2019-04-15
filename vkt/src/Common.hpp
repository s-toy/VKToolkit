#pragma once
#include <glm/glm.hpp>
#include "common/CommonMicro.h"
#include "common/UtilityInterface.h"

#define FORCE_ENABLE_DEBUG_UTILS

#if (defined(_DEBUG) || defined(DEBUG) || defined(FORCE_ENABLE_DEBUG_UTILS))
#	define _ENABLE_DEBUG_UTILS
#endif

#if (defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__))
#	define _WINDOWS
#endif

//TODO: some of these micros should be moved to hiveCommon

#define _OUTPUT_EVENT(e)			hiveUtility::hiveOutputEvent(e);
#define _OUTPUT_WARNING(e)			hiveUtility::hiveOutputWarning(__EXCEPTION_SITE__, (e));
#define _THROW_RUNTIME_ERROR(e)		throw std::runtime_error(e);

#define _DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete; \
    TypeName &operator =(const TypeName &) = delete;

#define _CALLBACK_0(Selector, Target, ...) std::bind(&Selector, Target, ##__VA_ARGS__)
#define _CALLBACK_1(Selector, Target, ...) std::bind(&Selector, Target, std::placeholders::_1, ##__VA_ARGS__)
#define _CALLBACK_2(Selector, Target, ...) std::bind(&Selector, Target, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define _CALLBACK_3(Selector, Target, ...) std::bind(&Selector, Target, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define _CALLBACK_4(Selector, Target, ...) std::bind(&Selector, Target, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)

namespace hiveVKT
{
	struct SWindowCreateInfo
	{
		int WindowWidth = 0, WindowHeight = 0;
		int WindowPosX = 0, WindowPosY = 0;
		std::string WindowTitle = "";
		bool IsWindowFullScreen = false;
		bool IsWindowResizable = false;

		bool isValid() const { return (WindowWidth > 0 && WindowHeight > 0); }	//TODO:
	};

	struct SViewPort
	{
		float Width = 0;
		float Height = 0;
	};

	struct SCameraInfo
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Up = glm::vec3(0.0f);
		glm::vec3 Front = glm::vec3(0.0f);
		float FOV = 0.0f;
		float NearPlane = 0.0f;
		float FarPlane = 0.0f;
	};

	struct SViewInfo
	{
		SViewPort ViewPortInfo;
		SCameraInfo CameraInfo;
	};

	struct STextureInfo
	{
		unsigned int Width = 0;
		unsigned int Height = 0;
		unsigned int InternalFormat = 0;
		unsigned int SourceType = 0;
		unsigned int TextureSourceFormat = 0;
	};

	struct STexture
	{
		STextureInfo TexInfo;
		std::shared_ptr<unsigned char> pTextureData = nullptr;
	};

	struct SGeometry
	{
		unsigned int VertexCount = 0;
		unsigned int IndexCount = 0;
		std::shared_ptr<float> pVertexData = nullptr;
		std::shared_ptr<unsigned int> pIndexData = nullptr;
	};

	struct SMesh
	{
		SGeometry Geometry;
		STexture Texture;
	};

	enum class EVertexComponent
	{
		VERTEX_COMPONENT_POSITION = 0x0,
		VERTEX_COMPONENT_NORMAL = 0x1,
		VERTEX_COMPONENT_COLOR = 0x2,
		VERTEX_COMPONENT_TEXCOORD = 0x3
	};

	struct SVertexLayout
	{
		std::vector<EVertexComponent> ComponentSet;
	};

	enum class ETextureType
	{
		TEXTURE_TYPE_DIFF = 0x0,
		TEXTURE_TYPE_SPEC = 0x1,
	};

	struct STextureDescriptorBindingInfo
	{
		std::vector<std::pair<ETextureType, uint32_t>> TextureDescriptorBindingInfo; //<texture type, binding>
	};
}