#include "vkrender/VulkanRenderTarget.h"

namespace vkrender
{

VulkanRenderTarget::VulkanRenderTarget( 
    const vk::ImageView& imgView, const vk::Format& imgFormat,
    const vk::SampleCountFlagBits& sampleCount
)
    :m_imgView{ imgView }
    ,m_imgFormat{ imgFormat }
    ,m_imgSampleCount{ sampleCount }
{}

VulkanRenderTarget::VulkanRenderTarget( VulkanTexture* pTexture )
    :m_imgView{ pTexture->imageView() }
    ,m_imgFormat{ pTexture->format() }
    ,m_imgSampleCount{ pTexture->sampleCount() }
{}

VulkanAttachmentBinding::VulkanAttachmentBinding()
    :m_imgFormat{}
    ,m_imgSampleCount{ vk::SampleCountFlagBits::e1 }
    ,m_referenceLayout{ vk::ImageLayout::eAttachmentOptimal }
    ,m_bUseAsResolveAttachment{ false }
    ,m_targetLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_targetStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_stencilLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_stencilStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_initialLayout{ vk::ImageLayout::eUndefined }
    ,m_finalLayout{ vk::ImageLayout::eUndefined }
{}

VulkanAttachmentBinding::VulkanAttachmentBinding(
    const vk::Format& imgFormat, const vk::SampleCountFlagBits& sampleCount,
    const vk::ImageLayout& attachmentLayout,
    const bool& bUseAsResolveAttachment
)
    :m_imgFormat{ imgFormat }
    ,m_imgSampleCount{ sampleCount }
    ,m_referenceLayout{ attachmentLayout }
    ,m_bUseAsResolveAttachment{ bUseAsResolveAttachment }
    ,m_targetLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_targetStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_stencilLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_stencilStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_initialLayout{ vk::ImageLayout::eUndefined }
    ,m_finalLayout{ vk::ImageLayout::eUndefined }
{}

void VulkanAttachmentBinding::setTargetSemantics(
    const vk::AttachmentLoadOp& targetLoadOp, const vk::AttachmentStoreOp& targetStoreOp,
    const vk::AttachmentLoadOp& stencilLoadOp, const vk::AttachmentStoreOp& stencilStoreOp
)
{
    m_targetLoadOp = targetLoadOp;
    m_targetStoreOp = targetStoreOp;
    m_stencilLoadOp = stencilLoadOp;
    m_stencilStoreOp = stencilStoreOp;
}

void VulkanAttachmentBinding::setTargetLayout(
    const vk::ImageLayout& initialLayout, const vk::ImageLayout& finalLayout
)
{
    m_initialLayout = initialLayout;
    m_finalLayout = finalLayout;
}

} // namespace vkrender
