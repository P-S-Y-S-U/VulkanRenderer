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
    VulkanRenderTarget( 
        const vk::ImageView& imgView, const vk::Format& imgFormat,
        const vk::SampleCountFlagBits& sampleCount,
        const bool& bUseAsResolveAttachment = false
    );
    VulkanRenderTarget(
        VulkanTexture* pTexture,
        const bool& m_bUseAsResolveAttachment = false
    );
    ~VulkanRenderTarget() = default;

    void setTargetSemantics(
        const vk::AttachmentLoadOp& targetLoadOp, const vk::AttachmentStoreOp& targetStoreOp,
        const vk::AttachmentLoadOp& stencilLoadOp, const vk::AttachmentStoreOp& stencilStoreOp
    );
    void setTargetLayout(
        const vk::ImageLayout& initialLayout, const vk::ImageLayout& finalLayout,
        const vk::ImageLayout& referenceLayout
    );

    vk::ImageView m_imgView;
    vk::Format m_imgFormat;
    vk::SampleCountFlagBits m_imgSampleCount;

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