#include "vkrender/VulkanRenderer.h"
#include "vkrender/VulkanDebugMessenger.h"
#include "vkrender/VulkanLayer.hpp"
#include "vkrender/VulkanWindow.h"

#include "utilities/VulkanLogger.h"

namespace vkrender
{

void populateDebugUtilsMessengerCreateInfo( vk::DebugUtilsMessengerCreateInfoEXT& vkDebugUtilsMessengerCreateInfo );
void logVulkanInstanceCreationInfo(const vk::InstanceCreateInfo &instanceCreateInfo);
bool checkValidationLayerSupport();

void VulkanRenderer::createInstance()
{
	using namespace vkrender;

	if (ENABLE_VALIDATION_LAYER && !checkValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested, not available");
	}

    vk::ApplicationInfo applicationInfo{};
	applicationInfo.pApplicationName = "Sample Application";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_3;
	applicationInfo.pNext = nullptr;

	vk::InstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	m_instanceExtensionContainer = VulkanWindow::populateAvailableExtensions();
	if( ENABLE_VALIDATION_LAYER )
	{
		m_instanceExtensionContainer.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	instanceCreateInfo.enabledExtensionCount = static_cast<std::uint32_t>(m_instanceExtensionContainer.size());
	instanceCreateInfo.ppEnabledExtensionNames = m_instanceExtensionContainer.data();
	vk::DebugUtilsMessengerCreateInfoEXT vkDebugUtilsMessengerCreateInfo{};
	if (ENABLE_VALIDATION_LAYER)
	{
		instanceCreateInfo.enabledLayerCount = static_cast<std::uint32_t>(layer::VALIDATION_LAYER.m_layers.size());
		instanceCreateInfo.ppEnabledLayerNames = layer::VALIDATION_LAYER.m_layers.data();
		populateDebugUtilsMessengerCreateInfo( vkDebugUtilsMessengerCreateInfo );
		instanceCreateInfo.pNext = &vkDebugUtilsMessengerCreateInfo;
	}
	else {
		instanceCreateInfo.enabledLayerCount = 0;
	}

	// creating Vulkan Instance
	if (vk::createInstance(&instanceCreateInfo, nullptr, &m_vkInstance) != vk::Result::eSuccess)
	{
		std::string errorMsg = "FAILED TO CREATE INSTANCE!";
		LOG_ERROR( "FAILED TO CREATE INSTANCE!" );
		throw std::runtime_error( errorMsg );
	}
	LOG_INFO( "Created Vulkan instance successfully" );
	logVulkanInstanceCreationInfo(instanceCreateInfo);
}

void VulkanRenderer::setupDebugMessenger()
{
	using namespace vkrender;
	vk::DebugUtilsMessengerCreateInfoEXT vkDebugUtilsMessengerCreateInfo{};
	populateDebugUtilsMessengerCreateInfo( vkDebugUtilsMessengerCreateInfo );
	if ( VulkanDebugMessenger::createDebugUtilsMessengerEXT(m_vkInstance, vkDebugUtilsMessengerCreateInfo, nullptr, m_vkDebugUtilsMessenger) != VK_SUCCESS )
	{
		throw std::runtime_error("failed to setup debug messenger!");
	}
}

void populateDebugUtilsMessengerCreateInfo( vk::DebugUtilsMessengerCreateInfoEXT& vkDebugUtilsMessengerCreateInfo )
{
	using namespace vkrender;

	vkDebugUtilsMessengerCreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	vkDebugUtilsMessengerCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
	vkDebugUtilsMessengerCreateInfo.pfnUserCallback = VulkanDebugMessenger::debugCallback;
}

void logVulkanInstanceCreationInfo(const vk::InstanceCreateInfo &instanceCreateInfo)
{
	LOG_INFO( fmt::format( "Enabled Extensions Count : {}", instanceCreateInfo.enabledExtensionCount) );

	for( auto i = 0u; i < instanceCreateInfo.enabledExtensionCount; i++ )
	{
		LOG_INFO( fmt::format("EXTENSION: {}", instanceCreateInfo.ppEnabledExtensionNames[i]) );
	}

	LOG_INFO( fmt::format("Enabled Layers Count : {}", instanceCreateInfo.enabledLayerCount) );
	for( auto i = 0u; i < instanceCreateInfo.enabledLayerCount; i++ )
	{
		LOG_INFO( fmt::format("LAYER: {}", instanceCreateInfo.ppEnabledLayerNames[i]) );
	}
}

bool checkValidationLayerSupport()
{
	using namespace vkrender;

	// Enumerating Insatnce layer properties
	std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

	// validating Layer support
	for (const auto& layerName : layer::VALIDATION_LAYER.m_layers)
	{
		bool bLayerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				bLayerFound = true;
			}
		}
		if (!bLayerFound) { return false;  }
	}

	return true;
}

}