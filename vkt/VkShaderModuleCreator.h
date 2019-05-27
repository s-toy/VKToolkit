#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"
#include "Common.h"

namespace hiveVKT
{
	class CVkShaderModuleCreator
	{
		using SprivCodeType = std::vector<uint32_t>;

	public:
		vk::ShaderModule create(const vk::Device& vDevice, const SprivCodeType& vSpvCode);
		vk::ShaderModule create(const vk::Device& vDevice, const std::string& vSpvFileName);
		vk::UniqueShaderModule createUnique(const vk::Device& vDevice, const SprivCodeType& vSpvCode);
		vk::UniqueShaderModule createUnique(const vk::Device& vDevice, const std::string& vSpvFileName);

	private:
		vk::ShaderModuleCreateInfo m_ShaderModuleCreateInfo;

		void __prepareShaderModuleCreateInfo(const SprivCodeType& vSpvCode);
		SprivCodeType __readSpvFile(const std::string& vSpvFileName);
	};
}