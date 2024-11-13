#ifndef VKRENDER_VULKAN_TEXTURE_MANAGER_H
#define VKRENDER_VULKAN_TEXTURE_MANAGER_H

#include "vkrender/VulkanRenderer.h"
#include "vkrender/VulkanRendererExports.hpp"

#include "utilities/Image.h"

#include <vulkan/vulkan.hpp>
#include <vector>

namespace vkrender
{

class VulkanTexture;

class VULKANRENDERER_EXPORTS VulkanTextureManager
{
public:
    VulkanTextureManager( vkrender::VulkanRenderer* pVkRenderer );
    ~VulkanTextureManager();

    VulkanTexture* createTexture(
        const utils::Dimension& texDimension, const std::uint32_t& miplevels,
        const vk::Format& imgFormat, const vk::ImageTiling& imgTiling,
        const vk::ImageUsageFlags& imgUsageFlags, const vk::MemoryPropertyFlags& memoryPropertyFlags,
        const vk::SampleCountFlagBits& imgSampleCountFlags,
        const vk::ImageAspectFlags& imgAspect
    );
    VulkanTexture* createTextureAndUploadBuffer(
        const utils::Image& img, 
        const vk::Format& imgFormat, const vk::ImageTiling& imgTiling,
        const vk::ImageUsageFlags& imgUsageFlags, const vk::MemoryPropertyFlags& memoryPropertyFlags,
        const vk::SampleCountFlagBits& imgSampleCountFlags,
        const vk::ImageAspectFlags& imgAspect
    );

    void transitionImageLayout(
        VulkanTexture* pTexture,
        const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout
    );

    void transferImgBufferToTexture(
        const utils::Image& img, VulkanTexture* pTexture
    );

    void generateMipmaps( VulkanTexture* pTexture );
    
    VulkanRenderer* getRenderer() const { return m_pVkRenderer; }
    vk::Device* getDevice() const { return &m_pVkRenderer->m_vkLogicalDevice; }
private:
    vkrender::VulkanRenderer* m_pVkRenderer;

    std::vector<VulkanTexture> m_textureArray;
};

} // namespace vkrender

#endif