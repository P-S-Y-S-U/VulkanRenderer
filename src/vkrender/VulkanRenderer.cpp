#include "vkrender/VulkanRenderer.h"
#include "vkrender/VulkanDebugMessenger.h"

#include "utilities/VulkanLogger.h"

#include <vulkan/vulkan_wayland.h>

namespace vkrender
{

VulkanRenderer::VulkanRenderer()
{}

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
	m_vkInstance.destroySurfaceKHR( m_vkSurface );

	if( ENABLE_VALIDATION_LAYER )
	{
		VulkanDebugMessenger::destroyDebugUtilsMessengerEXT( m_vkInstance, m_vkDebugUtilsMessenger, nullptr );
	}
	m_vkInstance.destroy();
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