#include "VkShaderModuleCreator.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::ShaderModule hiveVKT::CVkShaderModuleCreator::create(const vk::Device& vDevice, const SprivCodeType& vSpvCode)
{
	__prepareShaderModuleCreateInfo(vSpvCode);
	return vDevice.createShaderModule(m_ShaderModuleCreateInfo, nullptr);
}

//***********************************************************************************************
//FUNCTION:
vk::UniqueShaderModule hiveVKT::CVkShaderModuleCreator::createUnique(const vk::Device& vDevice, const SprivCodeType& vSpvCode)
{
	__prepareShaderModuleCreateInfo(vSpvCode);
	return vDevice.createShaderModuleUnique(m_ShaderModuleCreateInfo, nullptr);
}

//***********************************************************************************************
//FUNCTION:
vk::UniqueShaderModule hiveVKT::CVkShaderModuleCreator::createUnique(const vk::Device& vDevice, const std::string& vSpvFileName)
{
	return createUnique(vDevice, __readSpvFile(vSpvFileName));
}

//***********************************************************************************************
//FUNCTION:
vk::ShaderModule hiveVKT::CVkShaderModuleCreator::create(const vk::Device& vDevice, const std::string& vSpvFileName)
{
	return create(vDevice, __readSpvFile(vSpvFileName));
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkShaderModuleCreator::__prepareShaderModuleCreateInfo(const SprivCodeType& vSpvCode)
{
	_ASSERTE(!vSpvCode.empty());
	m_ShaderModuleCreateInfo.codeSize = vSpvCode.size() * sizeof(SprivCodeType::value_type);
	m_ShaderModuleCreateInfo.pCode = vSpvCode.data();
}

//***********************************************************************************************
//FUNCTION:
hiveVKT::CVkShaderModuleCreator::SprivCodeType hiveVKT::CVkShaderModuleCreator::__readSpvFile(const std::string& vSpvFileName)
{
	SprivCodeType Buffer;
	if (!readFile(vSpvFileName, Buffer)) _THROW_RUNTIME_ERROR(format("Failed to read file: %s", vSpvFileName.c_str()));
	return Buffer;
}