#pragma once
#include <Assimp\Importer.hpp>
#include <Assimp\scene.h>
#include <Assimp\postprocess.h>
#include "VkTexture2D.hpp"
#include "Mesh.hpp"

namespace hiveVKT
{
#define DEFAULT_MODEL_LOADING_FLAGS aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace

	struct SVkTextureInfo
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
		void loadModel(const std::vector<std::weak_ptr<SMesh>>& vData, const SVertexLayout& vVertexLayout, const STextureDescriptorBindingInfo& vTextureDescriptorBindingInfo);

		void processModel(vk::Device vDevice, vk::CommandPool vCommandPool, vk::Queue vQueue);

		void updateModelData(const std::vector<std::weak_ptr<SMesh>>& vData);

		const vk::DescriptorSetLayout& getModelDescriptorSetLayout()const { return m_pDescriptorSetLayout; }

		void draw(const vk::CommandBuffer vCommandBuffer, const vk::PipelineLayout vPipelineLayout, const std::vector<vk::DescriptorSet>& vOtherDescriptorSet2BeBound);

		void destroy(vk::Device vDevice);

	private:
		_DISALLOW_COPY_AND_ASSIGN(CModel);

		const aiScene* m_pScene = nullptr;
		std::vector<std::weak_ptr<SMesh>> m_ModelData;

		std::string m_Directory = "";

		SVertexLayout m_VertexLayout;
		STextureDescriptorBindingInfo m_TextureDescriptorBindingInfo;

		std::vector<CMesh*> m_MeshSet;
		std::vector<SVkTextureInfo*> m_TextureSet;

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