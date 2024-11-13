#ifndef VKRENDER_VULKAN_TEXTURE_H
#define VKRENDER_VULKAN_TEXTURE_H

#include "vkrender/VulkanTextureManager.h"
#include "vkrender/VulkanRendererExports.hpp"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanTexture
{
public:
    VulkanTexture(
        VulkanTextureManager* pTextureManager,
        const utils::Dimension& texDimension, const std::uint32_t& miplevels,
        const vk::Format& imgFormat, const vk::ImageTiling& imgTiling,
        const vk::ImageUsageFlags& imgUsageFlags, const vk::MemoryPropertyFlags& memoryPropertyFlags,
        const vk::SampleCountFlagBits& imgSampleCountFlags,
        const vk::ImageAspectFlags& imgAspect
    );
    ~VulkanTexture();

    void createImage();
    void createImageView();

    vk::Format format() const { return m_vkImgFormat; }
    vk::SampleCountFlagBits sampleCount() const { return m_vkImgSampleCountFlags; }
private:
    VulkanTextureManager* m_pTextureManager;

    vk::Image m_vkImage;
    vk::ImageView m_vkImageView;
    vk::DeviceMemory m_vkImgMemory;

    vk::Format m_vkImgFormat;
    vk::ImageTiling m_vkImgTiling;
    vk::ImageUsageFlags m_vkImgUsageFlags;
    vk::MemoryPropertyFlags m_vkImgMemoryFlags;
    vk::SampleCountFlagBits m_vkImgSampleCountFlags;
    vk::ImageAspectFlags m_vkImgAspect;

    std::uint32_t m_miplevels;
    utils::Dimension m_texDimension;

    friend class VulkanTextureManager;
};

} // namespace vkrender

#endif