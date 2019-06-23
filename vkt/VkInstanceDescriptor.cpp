#include "VkInstanceDescriptor.h"
#include "VkInitializer.h"

using namespace hiveVKT;

hiveVKT::CVkInstanceDescriptor::CVkInstanceDescriptor()
{
	m_ApplicationInfo = initializer::applicationInfo();
}

hiveVKT::CVkInstanceDescriptor::~CVkInstanceDescriptor()
{
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkInstanceDescriptor::__assemblingInstanceCreateInfo()
{
#ifdef _DEBUG
	if (!__checkInstanceLayersAndExtensionsSupport())
		throw std::runtime_error("Not all requested instance layers and extensions are available.");
#endif

	m_InstanceCreateInfo.pApplicationInfo = &m_ApplicationInfo;
	m_InstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensionSet.size());
	m_InstanceCreateInfo.ppEnabledExtensionNames = m_InstanceExtensionSet.data();
	m_InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_InstanceLayerSet.size());
	m_InstanceCreateInfo.ppEnabledLayerNames = m_InstanceLayerSet.data();
}

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::CVkInstanceDescriptor::__checkInstanceLayersAndExtensionsSupport() const
{
	auto InstanceLayerPropertiesSet = vk::enumerateInstanceLayerProperties();
	auto InstanceExtensionPropertiesSet = vk::enumerateInstanceExtensionProperties();

	std::set<std::string> RequiredInstanceLayerSet(m_InstanceLayerSet.begin(), m_InstanceLayerSet.end());
	std::set<std::string> RequiredInstanceExtensionSet(m_InstanceExtensionSet.begin(), m_InstanceExtensionSet.end());
	for (const auto& LayerProperty : InstanceLayerPropertiesSet) RequiredInstanceLayerSet.erase(LayerProperty.layerName);
	for (const auto& ExtensionProperty : InstanceExtensionPropertiesSet) RequiredInstanceExtensionSet.erase(ExtensionProperty.extensionName);

	return RequiredInstanceLayerSet.empty() && RequiredInstanceExtensionSet.empty();
}