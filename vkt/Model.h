#pragma once
#include <Assimp\Importer.hpp>
#include <Assimp\scene.h>
#include <Assimp\postprocess.h>
#include "VkTexture2D.h"
#include "Mesh.h"

namespace hiveVKT
{
#define DEFAULT_MODEL_LOADING_FLAGS aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace

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

	struct STextureInfo
	{
		CVkTexture2D Texture;

		ETextureType  TextureType;
		std::string   TextureName;
	};

	class CModel
	{
	public:
		CModel() = default;

		void loadModel(std::string vFilePath, const SVertexLayout& vVertexLayout, const STextureDescriptorBindingInfo& vTextureDescriptorBindingInfo);

		const vk::DescriptorSetLayout& getModelDescriptorSetLayout()const { return m_pDescriptorSetLayout; }

		void draw(const vk::CommandBuffer vCommandBuffer, const vk::PipelineLayout vPipelineLayout, const std::vector<vk::DescriptorSet>& vOtherDescriptorSet2BeBound);

		void destroy(vk::Device vDevice);

	private:
		_DISALLOW_COPY_AND_ASSIGN(CModel);

		std::string m_Directory = "";

		SVertexLayout m_VertexLayout;
		STextureDescriptorBindingInfo m_TextureDescriptorBindingInfo;

		std::vector<CMesh*> m_MeshSet;
		std::vector<STextureInfo*> m_TextureSet;

		vk::DescriptorSetLayout m_pDescriptorSetLayout;
		vk::DescriptorPool m_pDescriptorPool;

		void __createVulkanResource(unsigned int vNumMesh);
		void __processNodes(const aiNode* vNode, const aiScene* vScene);
		void __processMesh(const aiMesh* vMesh, const aiScene* vScene);

		int __createNewTexture(const std::string& vTextureName, ETextureType vTextureType);
		int __loadMaterialTextures(const aiMaterial* vMaterial, ETextureType vTextureType);
		int __getTextureIndex(const std::string& vTextureName);

		vk::DescriptorSet __createDescriptorSet(const std::vector<int>& vTextureIndexSet, const std::vector<int>& vTextureBindingInfoSet);
	};
}