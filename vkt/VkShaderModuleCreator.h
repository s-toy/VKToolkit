#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"
#include "Common.h"
#include "Export.h"

namespace hiveVKT
{
	class VKT_DECLSPEC CVkShaderModuleCreator
	{
		using SprivCodeType = std::vector<uint32_t>;

	public:
		vk::Result create(const std::string& vSpvFileName, vk::ShaderModule& voShaderModule);
		vk::UniqueShaderModule createUnique(const std::string& vSpvFileName);

	private:
		vk::ShaderModuleCreateInfo m_ShaderModuleCreateInfo;

		SprivCodeType __readSpvFile(const std::string& vSpvFileName);
		bool __verifySpvCode(const SprivCodeType& vSpvCode);
		void __prepareShaderModuleCreateInfo(const SprivCodeType& vSpvCode);
	};
}