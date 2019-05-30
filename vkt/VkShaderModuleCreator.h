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
		vk::ShaderModule create(const std::string& vSpvFileName);
		vk::UniqueShaderModule createUnique(const std::string& vSpvFileName);

	private:
		vk::ShaderModuleCreateInfo m_ShaderModuleCreateInfo;

		SprivCodeType __readSpvFile(const std::string& vSpvFileName);
		bool __verifySpvCode(const SprivCodeType& vSpvCode);
		void __prepareShaderModuleCreateInfo(const SprivCodeType& vSpvCode);
	};
}