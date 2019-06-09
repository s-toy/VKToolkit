#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "VkContext.h"

//************************************************************************************
//Function:
void hiveVKT::CModel::loadModel(std::string vFilePath, const SVertexLayout& vVertexLayout, const STextureDescriptorBindingInfo& vTextureDescriptorBindingInfo)
{
	_ASSERT(CVkContext::getInstance()->isContextCreated());
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(vFilePath, DEFAULT_MODEL_LOADING_FLAGS);

	if (!Scene || Scene->mFlags&AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
		_ASSERT(false);

	m_Directory = vFilePath.substr(0, vFilePath.find_last_of('/'));
	m_VertexLayout = vVertexLayout;
	m_TextureDescriptorBindingInfo = vTextureDescriptorBindingInfo;

	__createVulkanResource(Scene->mNumMeshes);

	__processNodes(Scene->mRootNode, Scene);
}

//************************************************************************************
//Function:
void hiveVKT::CModel::draw(const vk::CommandBuffer vCommandBuffer, const vk::PipelineLayout vPipelineLayout, const std::vector<vk::DescriptorSet>& vOtherDescriptorSet2BeBound)
{
	for (auto Mesh : m_MeshSet)
		Mesh->draw(vCommandBuffer, vPipelineLayout, vOtherDescriptorSet2BeBound);
}

//************************************************************************************
//Function:
void hiveVKT::CModel::destroy(vk::Device vDevice)
{
	for (auto Mesh : m_MeshSet)
	{
		Mesh->destroy(vDevice);
		delete Mesh;
	}

	for (auto TextureInfo : m_TextureSet)
	{
		TextureInfo->Texture.destroy();
		delete TextureInfo;
	}

	vDevice.destroyDescriptorPool(m_pDescriptorPool);
	vDevice.destroyDescriptorSetLayout(m_pDescriptorSetLayout);
}

//************************************************************************************
//Function:
void hiveVKT::CModel::__createVulkanResource(unsigned int vNumMesh)
{
	auto Device = CVkContext::getInstance()->getVulkanDevice();

	std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindingSet;
	for (auto BindingInfo : m_TextureDescriptorBindingInfo.TextureDescriptorBindingInfo)
	{
		uint32_t BidingPoint = BindingInfo.second;

		vk::DescriptorSetLayoutBinding Binding = {};
		Binding.binding = BidingPoint;
		Binding.descriptorCount = 1;
		Binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		Binding.stageFlags = vk::ShaderStageFlagBits::eFragment;

		DescriptorSetLayoutBindingSet.emplace_back(Binding);
	}

	vk::DescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
	DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(DescriptorSetLayoutBindingSet.size());
	DescriptorSetLayoutCreateInfo.pBindings = DescriptorSetLayoutBindingSet.data();

	m_pDescriptorSetLayout = Device.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo);

	std::array<vk::DescriptorPoolSize, 1> DescriptorPoolSizeSet = {};
	DescriptorPoolSizeSet[0].type = vk::DescriptorType::eCombinedImageSampler;
	DescriptorPoolSizeSet[0].descriptorCount = static_cast<uint32_t>(vNumMesh * m_TextureDescriptorBindingInfo.TextureDescriptorBindingInfo.size());

	vk::DescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
	DescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(DescriptorPoolSizeSet.size());
	DescriptorPoolCreateInfo.pPoolSizes = DescriptorPoolSizeSet.data();
	DescriptorPoolCreateInfo.maxSets = vNumMesh;

	m_pDescriptorPool = Device.createDescriptorPool(DescriptorPoolCreateInfo);
}

//************************************************************************************
//Function:
void hiveVKT::CModel::__processNodes(const aiNode* vNode, const aiScene* vScene)
{
	for (unsigned int i = 0; i < vNode->mNumMeshes; ++i)
	{
		aiMesh* Mesh = vScene->mMeshes[vNode->mMeshes[i]];
		__processMesh(Mesh, vScene);
	}

	for (unsigned int i = 0; i < vNode->mNumChildren; ++i)
	{
		__processNodes(vNode->mChildren[i], vScene);
	}
}

//************************************************************************************
//Function:
void hiveVKT::CModel::__processMesh(const aiMesh* vMesh, const aiScene* vScene)
{
	std::vector<float> VertexData;
	std::vector<uint32_t> IndexData;

	for (unsigned int i = 0; i < vMesh->mNumVertices; ++i)
	{
		for (auto& Component : m_VertexLayout.ComponentSet)
		{
			switch (Component)
			{
			case EVertexComponent::VERTEX_COMPONENT_POSITION:
				VertexData.push_back(vMesh->mVertices[i].x);
				VertexData.push_back(vMesh->mVertices[i].y);
				VertexData.push_back(vMesh->mVertices[i].z);
				break;
			case EVertexComponent::VERTEX_COMPONENT_NORMAL:
				VertexData.push_back(vMesh->mNormals[i].x);
				VertexData.push_back(vMesh->mNormals[i].y);
				VertexData.push_back(vMesh->mNormals[i].z);
				break;
			case EVertexComponent::VERTEX_COMPONENT_COLOR:
				VertexData.push_back(0.0);
				VertexData.push_back(0.0);
				VertexData.push_back(0.0);
				break;
			case EVertexComponent::VERTEX_COMPONENT_TEXCOORD:
				VertexData.push_back(vMesh->mTextureCoords[0][i].x);
				VertexData.push_back(vMesh->mTextureCoords[0][i].y);
				break;
			default:
				_ASSERT(false); break;
			}
		}
	}

	for (unsigned int i = 0; i < vMesh->mNumFaces; ++i)
	{
		aiFace Face = vMesh->mFaces[i];
		for (unsigned int k = 0; k < Face.mNumIndices; ++k)
		{
			IndexData.push_back(Face.mIndices[k]);
		}
	}

	aiMaterial* Material = vScene->mMaterials[vMesh->mMaterialIndex];
	std::vector<int> TextureIndexSet;
	std::vector<int> BindingInfoSet;

	for (auto BindingInfo : m_TextureDescriptorBindingInfo.TextureDescriptorBindingInfo)
	{
		int TextureIndex = __loadMaterialTextures(Material, BindingInfo.first);

		if (TextureIndex >= 0)
		{
			TextureIndexSet.emplace_back(TextureIndex);
			BindingInfoSet.emplace_back(BindingInfo.second);
		}
	}

	vk::DescriptorSet DescriptorSet = __createDescriptorSet(TextureIndexSet, BindingInfoSet);

	CMesh* Mesh = new CMesh(VertexData, IndexData, DescriptorSet);
	m_MeshSet.push_back(Mesh);
}

//************************************************************************************
//Function:
int hiveVKT::CModel::__createNewTexture(const std::string& vTextureName, ETextureType vTextureType)
{
	int TextureWidth = 0, TextureHeight = 0, TextureChannels = 0;
	unsigned char* Pixels = stbi_load(vTextureName.c_str(), &TextureWidth, &TextureHeight, &TextureChannels, STBI_rgb_alpha);

	if (!Pixels)
	{
		stbi_image_free(Pixels);
		return -1;
	}

	vk::DeviceSize Size = static_cast<vk::DeviceSize>(TextureWidth * TextureHeight * 4);

	STextureInfo* TextureInfo = new STextureInfo();
	TextureInfo->Texture.create(TextureWidth, TextureHeight, vk::Format::eR8G8B8A8Unorm, 3, Size, Pixels);
	TextureInfo->TextureName = vTextureName;
	TextureInfo->TextureType = vTextureType;

	m_TextureSet.emplace_back(TextureInfo);

	stbi_image_free(Pixels);

	return static_cast<int>(m_TextureSet.size() - 1);
}

//************************************************************************************
//Function:
int hiveVKT::CModel::__loadMaterialTextures(const aiMaterial* vMaterial, ETextureType vTextureType)
{
	aiTextureType AiTextureType;

	switch (vTextureType)
	{
	case hiveVKT::ETextureType::TEXTURE_TYPE_DIFF:
		AiTextureType = aiTextureType_DIFFUSE;
		break;
	case hiveVKT::ETextureType::TEXTURE_TYPE_SPEC:
		AiTextureType = aiTextureType_SPECULAR;
		break;
	default:
		_ASSERT(false); break;
	}

	if (vMaterial->GetTextureCount(AiTextureType) == 0)
		return -1;

	aiString Texture;
	vMaterial->GetTexture(AiTextureType, 0, &Texture);
	std::string TextureName = m_Directory + "/" + Texture.C_Str();

	int Index = __getTextureIndex(TextureName);
	if (Index < 0)
		Index = __createNewTexture(TextureName, vTextureType);

	return Index;
}

//************************************************************************************
//Function:
int hiveVKT::CModel::__getTextureIndex(const std::string& vTextureName)
{
	for (auto i = 0; i < m_TextureSet.size(); ++i)
	{
		if (m_TextureSet[i]->TextureName == vTextureName)
			return i;
	}

	return -1;
}

//************************************************************************************
//Function:
vk::DescriptorSet hiveVKT::CModel::__createDescriptorSet(const std::vector<int>& vTextureIndexSet, const std::vector<int>& vTextureBindingInfoSet)
{
	_ASSERT(vTextureIndexSet.size() == vTextureBindingInfoSet.size());

	auto Device = CVkContext::getInstance()->getVulkanDevice();

	vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
	DescriptorSetAllocateInfo.descriptorPool = m_pDescriptorPool;
	DescriptorSetAllocateInfo.descriptorSetCount = 1;
	DescriptorSetAllocateInfo.pSetLayouts = &m_pDescriptorSetLayout;

	std::vector<vk::DescriptorSet> DescriptorSets;
	DescriptorSets = Device.allocateDescriptorSets(DescriptorSetAllocateInfo);

	std::vector<vk::DescriptorImageInfo> DescriptorImageInfos(vTextureIndexSet.size());
	std::vector<vk::WriteDescriptorSet> WriteDescriptorSetInfos(vTextureIndexSet.size());

	for (auto i = 0; i < vTextureIndexSet.size(); ++i)
	{
		vk::DescriptorImageInfo DescriptorImageInfo = {};
		DescriptorImageInfo.imageView = m_TextureSet[vTextureIndexSet[i]]->Texture.getImageView();
		DescriptorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		DescriptorImageInfo.sampler = m_TextureSet[vTextureIndexSet[i]]->Texture.getSampler();
		DescriptorImageInfos[i] = DescriptorImageInfo;

		vk::WriteDescriptorSet WriteDescriptorSet = {};
		WriteDescriptorSet.dstSet = DescriptorSets[0];
		WriteDescriptorSet.dstBinding = vTextureBindingInfoSet[i];
		WriteDescriptorSet.dstArrayElement = 0;
		WriteDescriptorSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		WriteDescriptorSet.descriptorCount = 1;
		WriteDescriptorSet.pImageInfo = &DescriptorImageInfos[i];
		WriteDescriptorSetInfos[i] = WriteDescriptorSet;
	}

	Device.updateDescriptorSets(WriteDescriptorSetInfos, nullptr);

	return DescriptorSets[0];
}
