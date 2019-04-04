#pragma once
#include <Assimp\Importer.hpp>
#include <Assimp\scene.h>
#include <Assimp\postprocess.h>
#include "VkTexture2D.hpp"
#include "Mesh.hpp"

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

	class VKT_DECLSPEC CModel
	{
	public:
		CModel() = default;

		void loadModel(std::string vFilePath, const SVertexLayout& vVertexLayout, const STextureDescriptorBindingInfo& vTextureDescriptorBindingInfo, vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue);

		const vk::DescriptorSetLayout& getModelDescriptorSetLayout()const { return m_pDescriptorSetLayout; }

		void draw(const vk::CommandBuffer vCommandBuffer, const vk::PipelineLayout vPipelineLayout);

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

		void __createVulkanResource(vk::Device vDevice, unsigned int vNumMesh);
		void __processNodes(const aiNode* vNode, const aiScene* vScene, vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue);
		void __processMesh(const aiMesh* vMesh, const aiScene* vScene, vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue);

		int __createNewTexture(const std::string& vTextureName, ETextureType vTextureType, vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue);
		int __loadMaterialTextures(const aiMaterial* vMaterial, ETextureType vTextureType, vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue);
		int __getTextureIndex(const std::string& vTextureName);

		vk::DescriptorSet __createDescriptorSet(vk::Device vDevice, const std::vector<int>& vTextureIndexSet, const std::vector<int>& vTextureBindingInfoSet);
	};
}