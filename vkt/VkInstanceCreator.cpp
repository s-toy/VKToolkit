#include "VkInstanceCreator.h"

using namespace hiveVKT;

hiveVKT::CVkInstanceCreator::CVkInstanceCreator()
{
	__init();
}

//***********************************************************************************************
//FUNCTION:
vk::Instance hiveVKT::CVkInstanceCreator::create()
{
	__prepareInstanceCreateInfo();
	return vk::createInstance(m_InstanceCreateInfo, nullptr);
}

//***********************************************************************************************
//FUNCTION:
vk::UniqueInstance hiveVKT::CVkInstanceCreator::createUnique()
{
	__prepareInstanceCreateInfo();
	return vk::createInstanceUnique(m_InstanceCreateInfo, nullptr);
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkInstanceCreator::__init()
{
	m_ApplicationInfo.pApplicationName = "hiveApplication";
	m_ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	m_ApplicationInfo.pEngineName = "hiveVKT";
	m_ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	m_ApplicationInfo.apiVersion = VK_API_VERSION_1_1;
}

//***********************************************************************************************
//FUNCTION:
void hiveVKT::CVkInstanceCreator::__prepareInstanceCreateInfo()
{
	__addDebugLayersAndExtensionsIfNecessary();

#ifdef _ENABLE_DEBUG_UTILS
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
void hiveVKT::CVkInstanceCreator::__addDebugLayersAndExtensionsIfNecessary()
{
#ifdef _ENABLE_DEBUG_UTILS
	m_InstanceLayerSet.emplace_back("VK_LAYER_LUNARG_standard_validation");
	m_InstanceExtensionSet.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
}

//***********************************************************************************************
//FUNCTION:
bool hiveVKT::CVkInstanceCreator::__checkInstanceLayersAndExtensionsSupport() const
{
	auto InstanceLayerPropertiesSet = vk::enumerateInstanceLayerProperties();
	auto InstanceExtensionPropertiesSet = vk::enumerateInstanceExtensionProperties();

	std::set<std::string> RequiredInstanceLayerSet(m_InstanceLayerSet.begin(), m_InstanceLayerSet.end());
	std::set<std::string> RequiredInstanceExtensionSet(m_InstanceExtensionSet.begin(), m_InstanceExtensionSet.end());
	for (const auto& LayerProperty : InstanceLayerPropertiesSet) RequiredInstanceLayerSet.erase(LayerProperty.layerName);
	for (const auto& ExtensionProperty : InstanceExtensionPropertiesSet) RequiredInstanceExtensionSet.erase(ExtensionProperty.extensionName);

	return RequiredInstanceLayerSet.empty() && RequiredInstanceExtensionSet.empty();
}