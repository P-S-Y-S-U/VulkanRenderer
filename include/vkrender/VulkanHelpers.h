#ifndef VKRENDER_VULKAN_HELPERS_H
#define VKRENDER_VULKAN_HELPERS_H

#include "vkrender/VulkanRendererExports.hpp"
#include "vkrender/VulkanQueueFamily.hpp"
#include "vkrender/VulkanSwapChainStructs.hpp"
#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanHelpers
{
public:
    static QueueFamilyIndices findQueueFamilyIndices( const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR* pVkSurface );
    static SwapChainSupportDetails querySwapChainSupport( const vk::PhysicalDevice& vkPhysicalDevice, const vk::SurfaceKHR& vkSurface )
};

} // namespace vkrender

#endif