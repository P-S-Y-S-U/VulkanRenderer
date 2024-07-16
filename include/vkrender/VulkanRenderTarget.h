#ifndef VKRENDER_VULKAN_RENDER_TARGET_H
#define VKRENDER_VULKAN_RENDER_TARGET_H

#include "vkrender/VulkanRendererExports.hpp"
#include <vulkan/vulkan.hpp>

namespace vkrender
{

struct VULKANRENDERER_EXPORTS VulkanRenderTarget
{
public:
    VulkanRenderTarget();
    ~VulkanRenderTarget() = default;

    void setTargetStorage(
        const vk::Format& format, const vk::SampleCountFlagBits& sampleCount = vk::SampleCountFlagBits::e1
    );
    void setTargetSemantics(
        const vk::AttachmentLoadOp& targetLoadOp, const vk::AttachmentStoreOp& targetStoreOp,
        const vk::AttachmentLoadOp& stencilLoadOp, const vk::AttachmentStoreOp& stencilStoreOp
    );
    void setTargetLayout(
        const vk::ImageLayout& initialLayout, const vk::ImageLayout& finalLayout,
        const vk::ImageLayout& referenceLayout
    );

    vk::Format m_targetFormat;
    vk::SampleCountFlags m_targetSampleCount;

    vk::AttachmentLoadOp m_targetLoadOp;
    vk::AttachmentStoreOp m_targetStoreOp;
    vk::AttachmentLoadOp m_stencilLoadOp;
    vk::AttachmentStoreOp m_stencilStoreOp;

    vk::ImageLayout m_initialLayout;
    vk::ImageLayout m_finalLayout;
    vk::ImageLayout m_referenceLayout;
};

VulkanRenderTarget::VulkanRenderTarget()
    :m_targetFormat{ vk::Format::eUndefined }
    ,m_targetSampleCount{ vk::SampleCountFlagBits::e1 }
    ,m_targetLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_targetStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_stencilLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_stencilStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_initialLayout{ vk::ImageLayout::eUndefined }
    ,m_finalLayout{ vk::ImageLayout::eUndefined }
    ,m_referenceLayout{ vk::ImageLayout::eUndefined }
{}

void VulkanRenderTarget::setTargetStorage(
    const vk::Format& format, const vk::SampleCountFlagBits& sampleCount = vk::SampleCountFlagBits::e1
)
{
    m_targetFormat = format;
    m_targetSampleCount = sampleCount;
}

void VulkanRenderTarget::setTargetSemantics(
    const vk::AttachmentLoadOp& targetLoadOp, const vk::AttachmentStoreOp& targetStoreOp,
    const vk::AttachmentLoadOp& stencilLoadOp, const vk::AttachmentStoreOp& stencilStoreOp
)
{
    m_targetLoadOp = targetLoadOp;
    m_targetStoreOp = targetStoreOp;
    m_stencilLoadOp = stencilLoadOp;
    m_stencilStoreOp = stencilStoreOp;
}

void VulkanRenderTarget::setTargetLayout(
    const vk::ImageLayout& initialLayout, const vk::ImageLayout& finalLayout,
    const vk::ImageLayout& referenceLayout
)
{
    m_initialLayout = initialLayout;
    m_finalLayout = finalLayout;
    m_referenceLayout = referenceLayout;
}

} // namespace vkrender

#endif 