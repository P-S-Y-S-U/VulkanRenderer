#ifndef VKRENDER_VULKAN_RENDER_TARGET_H
#define VKRENDER_VULKAN_RENDER_TARGET_H

#include "vkrender/VulkanRendererExports.hpp"
#include "vkrender/VulkanTexture.h"
#include <vulkan/vulkan.hpp>

namespace vkrender
{

struct VULKANRENDERER_EXPORTS VulkanRenderTarget
{
public:
    VulkanRenderTarget( VulkanTexture* pTexture );
    ~VulkanRenderTarget() = default;

    void setTargetSemantics(
        const vk::AttachmentLoadOp& targetLoadOp, const vk::AttachmentStoreOp& targetStoreOp,
        const vk::AttachmentLoadOp& stencilLoadOp, const vk::AttachmentStoreOp& stencilStoreOp
    );
    void setTargetLayout(
        const vk::ImageLayout& initialLayout, const vk::ImageLayout& finalLayout,
        const vk::ImageLayout& referenceLayout
    );

    VulkanTexture* m_pTexture;

    vk::AttachmentLoadOp m_targetLoadOp;
    vk::AttachmentStoreOp m_targetStoreOp;
    vk::AttachmentLoadOp m_stencilLoadOp;
    vk::AttachmentStoreOp m_stencilStoreOp;

    vk::ImageLayout m_initialLayout;
    vk::ImageLayout m_finalLayout;
    vk::ImageLayout m_referenceLayout;

    bool m_bUseAsResolveAttachment;
};

} // namespace vkrender

#endif 