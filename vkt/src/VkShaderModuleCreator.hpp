#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.hpp"
#include "Common.hpp"

namespace hiveVKT
{
	class CVkShaderModuleCreator
	{
		using SprivCodeType = std::vector<uint32_t>;

	public:
		vk::ShaderModule create(const vk::Device& vDevice, const SprivCodeType& vSpvCode)
		{
			__prepareShaderModuleCreateInfo(vSpvCode);
			return vDevice.createShaderModule(m_ShaderModuleCreateInfo, nullptr);
		}

		vk::ShaderModule create(const vk::Device& vDevice, const std::string& vSpvFileName) { return create(vDevice, __readSpvFile(vSpvFileName)); }

		vk::UniqueShaderModule createUnique(const vk::Device& vDevice, const SprivCodeType& vSpvCode)
		{
			__prepareShaderModuleCreateInfo(vSpvCode);
			return vDevice.createShaderModuleUnique(m_ShaderModuleCreateInfo, nullptr);
		}

		vk::UniqueShaderModule createUnique(const vk::Device& vDevice, const std::string& vSpvFileName) { return createUnique(vDevice, __readSpvFile(vSpvFileName)); }

	private:
		vk::ShaderModuleCreateInfo m_ShaderModuleCreateInfo;

		void __prepareShaderModuleCreateInfo(const SprivCodeType& vSpvCode)
		{
			_ASSERTE(!vSpvCode.empty());
			m_ShaderModuleCreateInfo.codeSize = vSpvCode.size() * sizeof(SprivCodeType::value_type);
			m_ShaderModuleCreateInfo.pCode = vSpvCode.data();
		}

		SprivCodeType __readSpvFile(const std::string& vSpvFileName)
		{
			SprivCodeType Buffer;
			if (!readFile(vSpvFileName, Buffer)) _THROW_RUNTIME_ERROR(format("Failed to read file: %s", vSpvFileName.c_str()));
			return Buffer;
		}
	};
}