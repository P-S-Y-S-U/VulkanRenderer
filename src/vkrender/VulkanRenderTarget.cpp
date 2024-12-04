#include "vkrender/VulkanRenderTarget.h"

namespace vkrender
{

VulkanRenderTarget::VulkanRenderTarget( 
    const vk::ImageView& imgView, const vk::Format& imgFormat,
    const vk::SampleCountFlagBits& sampleCount,
    const bool& bUseAsResolveAttachment
)
    :m_imgView{ imgView }
    ,m_imgFormat{ imgFormat }
    ,m_imgSampleCount{ sampleCount }
    ,m_targetLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_targetStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_stencilLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_stencilStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_initialLayout{ vk::ImageLayout::eUndefined }
    ,m_finalLayout{ vk::ImageLayout::eUndefined }
    ,m_referenceLayout{ vk::ImageLayout::eUndefined }
    ,m_bUseAsResolveAttachment{ false }
{}

VulkanRenderTarget::VulkanRenderTarget( VulkanTexture* pTexture, const bool& bUseAsResolveAttachment )
    :m_imgView{ pTexture->imageView() }
    ,m_imgFormat{ pTexture->format() }
    ,m_imgSampleCount{ pTexture->sampleCount() }
    ,m_targetLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_targetStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_stencilLoadOp{ vk::AttachmentLoadOp::eDontCare }
    ,m_stencilStoreOp{ vk::AttachmentStoreOp::eDontCare }
    ,m_initialLayout{ vk::ImageLayout::eUndefined }
    ,m_finalLayout{ vk::ImageLayout::eUndefined }
    ,m_referenceLayout{ vk::ImageLayout::eUndefined }
    ,m_bUseAsResolveAttachment{ false }
{}

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
