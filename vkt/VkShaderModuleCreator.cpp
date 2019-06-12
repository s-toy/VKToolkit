#include "VkShaderModuleCreator.h"
#include "VkContext.h"

using namespace hiveVKT;

//***********************************************************************************************
//FUNCTION:
vk::UniqueShaderModule CVkShaderModuleCreator::createUnique(const std::string& vSpvFileName)
{
	SprivCodeType SpvCode = __readSpvFile(vSpvFileName);
	if (!__verifySpvCode(SpvCode))
	{
		_OUTPUT_WARNING("bad spv code!");
		return vk::UniqueShaderModule();
	}

	__prepareShaderModuleCreateInfo(SpvCode);

	_ASSERT(CVkContext::getInstance()->isContextCreated());
	return CVkContext::getInstance()->getVulkanDevice().createShaderModuleUnique(m_ShaderModuleCreateInfo);
}

//***********************************************************************************************
//FUNCTION:
vk::ShaderModule CVkShaderModuleCreator::create(const std::string& vSpvFileName)
{
	SprivCodeType SpvCode = __readSpvFile(vSpvFileName);
	if (!__verifySpvCode(SpvCode))
	{
		_OUTPUT_WARNING("bad spv code!");
		return vk::ShaderModule();
	}

	__prepareShaderModuleCreateInfo(SpvCode);
	_ASSERT(CVkContext::getInstance()->isContextCreated());

	return CVkContext::getInstance()->getVulkanDevice().createShaderModule(m_ShaderModuleCreateInfo);
}

//***********************************************************************************************
//FUNCTION:
bool CVkShaderModuleCreator::__verifySpvCode(const SprivCodeType& vSpvCode)
{
	if (vSpvCode.empty()) 
		return false;

	char MagicNumber[4];
	memcpy(MagicNumber, vSpvCode.data(), 4);
	//magic number can be used to weakly validate that the binary blob is a SPIR-V module.
	//magic number can be 0x03022307 or 0x07230203 depending on the endianness of the module and host
	if ((MagicNumber[0] == 0x03 && MagicNumber[1] == 0x02 && MagicNumber[2] == 0x23 && MagicNumber[3] == 0x07) ||
		(MagicNumber[0] == 0x07 && MagicNumber[1] == 0x23 && MagicNumber[2] == 0x02 && MagicNumber[3] == 0x03))
		return true;

	return false;
}

//***********************************************************************************************
//FUNCTION:
void CVkShaderModuleCreator::__prepareShaderModuleCreateInfo(const SprivCodeType& vSpvCode)
{
	_ASSERTE(!vSpvCode.empty());
	m_ShaderModuleCreateInfo.codeSize = vSpvCode.size() * sizeof(SprivCodeType::value_type);
	m_ShaderModuleCreateInfo.pCode = vSpvCode.data();
}

//***********************************************************************************************
//FUNCTION:
CVkShaderModuleCreator::SprivCodeType CVkShaderModuleCreator::__readSpvFile(const std::string& vSpvFileName)
{
	SprivCodeType Buffer;
	if (!readFile(vSpvFileName, Buffer)) _OUTPUT_WARNING(format("Failed to read file: %s", vSpvFileName.c_str()));
	return Buffer;
}