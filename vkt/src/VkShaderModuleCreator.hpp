#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.hpp"

namespace hiveVKT
{
	class CVkShaderModuleCreator
	{
	public:
		vk::ShaderModule create(const vk::Device& vDevice, const std::vector<char>& vSpvCode)
		{
			__prepareShaderModuleCreateInfo(vSpvCode);
			return vDevice.createShaderModule(m_ShaderModuleCreateInfo, nullptr);
		}

		vk::ShaderModule create(const vk::Device& vDevice, const std::string& vSpvFileName) { return create(vDevice, readFile(vSpvFileName)); }

		vk::UniqueShaderModule createUnique(const vk::Device& vDevice, const std::vector<char>& vSpvCode)
		{
			__prepareShaderModuleCreateInfo(vSpvCode);
			return vDevice.createShaderModuleUnique(m_ShaderModuleCreateInfo, nullptr);
		}

		vk::UniqueShaderModule createUnique(const vk::Device& vDevice, const std::string& vSpvFileName) { return createUnique(vDevice, readFile(vSpvFileName)); }

	private:
		vk::ShaderModuleCreateInfo m_ShaderModuleCreateInfo;

		void __prepareShaderModuleCreateInfo(const std::vector<char>& vSpvCode)
		{
			_ASSERTE(!vSpvCode.empty());
			m_ShaderModuleCreateInfo.codeSize = vSpvCode.size();
			m_ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vSpvCode.data());
		}
	};
}