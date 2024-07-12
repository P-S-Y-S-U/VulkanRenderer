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
    static SwapChainSupportDetails querySwapChainSupport( const vk::PhysicalDevice& vkPhysicalDevice, const vk::SurfaceKHR& vkSurface );
    static vk::SampleCountFlagBits getMaxUsableSampleCount( const vk::PhysicalDevice& vkPhysicalDevice );

    static std::uint32_t findMemoryType( 
        const vk::PhysicalDevice& vkPhysicalDevice,
        const std::uint32_t& typeFilter, const vk::MemoryPropertyFlags& propertyFlags
    );
    
    // Image Related
    static void createImage(
        const vk::PhysicalDevice& vkPhysicalDevice, const vk::Device& vkLogicalDevice,
        const std::uint32_t& width, const std::uint32_t& height, const std::uint32_t& mipmapLevels,
	    const vk::SampleCountFlagBits& numOfSamples,
        const vk::Format& format, const vk::ImageTiling& tiling,
        const vk::ImageUsageFlags& usageFlags, const vk::MemoryPropertyFlags& memPropFlags,
        vk::Image& image, vk::DeviceMemory& imageMemory
    );

    static vk::ImageView createImageView(
        const vk::Device& vkLogicalDevice,
	    const vk::Image& image,
	    const vk::Format& format,
	    const vk::ImageAspectFlags& aspect,
	    const std::uint32_t& mipmapLevels
    );
};

} // namespace vkrender

#endif