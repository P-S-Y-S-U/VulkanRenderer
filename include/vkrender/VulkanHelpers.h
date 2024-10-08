#ifndef VKRENDER_VULKAN_HELPERS_H
#define VKRENDER_VULKAN_HELPERS_H

#include "vkrender/VulkanRendererExports.hpp"
#include "vkrender/VulkanQueueFamily.hpp"
#include "vkrender/VulkanSwapChainStructs.hpp"
#include "vkrender/VulkanCommandBuffer.h"
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

    static void transitionImageLayout(
        VulkanCmdBuffer* pCmdBuffer,
        const vk::Image& image, const vk::Format& format, 
        const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout,
	    const std::uint32_t& mipmapLevels
    );
    
    static vk::Format findSupportedImgFormat(
        const vk::PhysicalDevice& vkPhysicalDevice,
        const std::initializer_list<vk::Format>& candidates,
        const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& features
    );

    static bool hasStencilComponent( const vk::Format& format );
};

} // namespace vkrender

#endif