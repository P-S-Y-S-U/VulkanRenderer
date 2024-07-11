#include "vkrender/VulkanRenderer.h"
#include "vkrender/VulkanDebugMessenger.h"

#include "utilities/VulkanLogger.h"

#include <vulkan/vulkan_wayland.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace vkrender
{

VulkanRenderer::VulkanRenderer()
{
    if (utils::VulkanRendererApiLogger::getSingletonPtr() == nullptr)
	{
		spdlog::sink_ptr consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		std::initializer_list<spdlog::sink_ptr> logSinks{
			consoleSink
		};
		utils::VulkanRendererApiLogger::createInstance(logSinks);
		utils::VulkanRendererApiLogger::getSingletonPtr()->getLogger()->set_level(spdlog::level::debug);
	}
}

VulkanRenderer::~VulkanRenderer()
{
    shutdown();
}

void VulkanRenderer::initVulkan( VulkanWindow* pVulkanWindow )
{
    createInstance();
	setupDebugMessenger();
	createSurface( pVulkanWindow );
	pickPhysicalDevice();
	createLogicalDevice();
}

void VulkanRenderer::shutdown()
{
    m_vkLogicalDevice.destroy();
	LOG_DEBUG("Logical Device Destroyed");

	m_vkInstance.destroySurfaceKHR( m_vkSurface );
	LOG_DEBUG("Vulkan Surface Destroyed");

	if( ENABLE_VALIDATION_LAYER )
	{
		VulkanDebugMessenger::destroyDebugUtilsMessengerEXT( m_vkInstance, m_vkDebugUtilsMessenger, nullptr );
		LOG_DEBUG("Debug Messenger Destroyed");
	}
	m_vkInstance.destroy();
	LOG_DEBUG("Vulkan Instance Destroyed");
}

void VulkanRenderer::createSurface( VulkanWindow* pVulkanWindow )
{
    m_pVulkanWindow = pVulkanWindow;

#ifdef _WIN32
	vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.sType = vk::StructureType::eWin32SurfaceCreateInfoKHR;
	surfaceCreateInfo.hwnd = m_pVulkanWindow->getHandle();
	surfaceCreateInfo.hinstance = GetModuleHandle( nullptr );

	m_vkSurface = m_vkInstance.createWin32SurfaceKHR( surfaceCreateInfo );
#else
	VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.surface = m_pVulkanWindow->getHandle();
	surfaceCreateInfo.display = m_pVulkanWindow->getDisplayHandle();
	surfaceCreateInfo.pNext = nullptr;

 	VkResult surfaceCreateReslt = vkCreateWaylandSurfaceKHR( 
		reinterpret_cast<const VkInstance&>( m_vkInstance ),
		&surfaceCreateInfo,
		nullptr,
		reinterpret_cast<VkSurfaceKHR*>( &m_vkSurface )
	);

	if( surfaceCreateReslt != VK_SUCCESS )
	{
		LOG_ERROR("Failed to Create Vulkan Wayland Surface");
	}

#endif
	LOG_INFO( "Vulkan Surface Created" );
}

} // namespace vkrender