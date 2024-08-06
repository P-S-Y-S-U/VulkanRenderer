#include "vkrender/VulkanRenderTarget.h"

namespace vkrender
{

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
    ,m_bUseAsResolveAttachment{ false }
{}

void VulkanRenderTarget::setTargetStorage(
    const vk::Format& format,
    const vk::SampleCountFlagBits& sampleCount,
    const bool& bUseAsResolveTarget
)
{
    m_targetFormat = format;
    m_targetSampleCount = sampleCount;
    m_bUseAsResolveAttachment = bUseAsResolveTarget;
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
