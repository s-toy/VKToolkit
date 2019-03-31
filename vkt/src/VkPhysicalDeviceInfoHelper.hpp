#pragma once
#include <vulkan/vulkan.hpp>
#include <common/Singleton.h>
#include "VKTExport.hpp"

namespace hiveVKT
{
	class VKT_DECLSPEC CVkPhysicalDeviceInfoHelper :public hiveDesignPattern::CSingleton<CVkPhysicalDeviceInfoHelper>
	{
	public:
		void init(vk::PhysicalDevice vPhysicalDevice)
		{
			m_pPhysicalDevice = vPhysicalDevice;
		}

		vk::PhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties()const { return m_pPhysicalDevice.getMemoryProperties(); }

	private:
		vk::PhysicalDevice m_pPhysicalDevice;

		friend class hiveDesignPattern::CSingleton<CVkPhysicalDeviceInfoHelper>;
	};
}