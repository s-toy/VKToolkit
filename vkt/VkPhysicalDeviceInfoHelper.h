#pragma once
#include <vulkan/vulkan.hpp>
#include <common/Singleton.h>

namespace hiveVKT
{
	class CVkPhysicalDeviceInfoHelper :public hiveDesignPattern::CSingleton<CVkPhysicalDeviceInfoHelper>
	{
	public:
		void init(vk::PhysicalDevice vPhysicalDevice);

		vk::PhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties()const { return m_pPhysicalDevice.getMemoryProperties(); }

	private:
		vk::PhysicalDevice m_pPhysicalDevice;

	friend class hiveDesignPattern::CSingleton<CVkPhysicalDeviceInfoHelper>;
	};
}