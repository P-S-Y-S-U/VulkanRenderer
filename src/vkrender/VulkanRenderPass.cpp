#include "vkrender/VulkanRenderPass.h"
#include "utilities/VulkanLogger.h"

namespace vkrender
{
VulkanRenderPass::VulkanRenderPass( vk::Device* pLogicalDevice, const std::string& renderPassName )
    :m_pLogicalDevice{ pLogicalDevice }
    ,m_renderPassName{ renderPassName }
{}

VulkanRenderPass::~VulkanRenderPass()
{
    destroyRenderPass();
}

void VulkanRenderPass::prepareTargetAttachments( const VulkanRenderTargetArray& attachments )
{
    m_targetAttachments.resize( attachments.size() );

    for( auto i = 0u; i < attachments.size(); i++ )
    {
        const VulkanRenderTarget& targetAttachment = attachments[i];

        vk::AttachmentDescription& attachmentDesc = m_targetAttachments[i];
        attachmentDesc.format = targetAttachment.m_imgFormat;
        attachmentDesc.samples = targetAttachment.m_imgSampleCount;
        attachmentDesc.loadOp = targetAttachment.m_targetLoadOp;
        attachmentDesc.storeOp = targetAttachment.m_targetStoreOp;
        attachmentDesc.stencilLoadOp = targetAttachment.m_stencilLoadOp;
        attachmentDesc.stencilStoreOp = targetAttachment.m_stencilStoreOp;
        attachmentDesc.initialLayout = targetAttachment.m_initialLayout;
        attachmentDesc.finalLayout = targetAttachment.m_finalLayout;

        vk::AttachmentReference attachmentRef{};
        attachmentRef.attachment = i;
        attachmentRef.layout = targetAttachment.m_referenceLayout;

        if( attachmentRef.layout == vk::ImageLayout::eColorAttachmentOptimal && !targetAttachment.m_bUseAsResolveAttachment )
            m_colorAttachmentRefs.push_back(attachmentRef);
        else if( attachmentRef.layout == vk::ImageLayout::eDepthStencilAttachmentOptimal )
            m_depthAttachmentRefs.push_back(attachmentRef);
        else if( attachmentRef.layout == vk::ImageLayout::eColorAttachmentOptimal && targetAttachment.m_bUseAsResolveAttachment )
            m_resolveAttachmentRefs.push_back(attachmentRef);
    }
}

std::uint32_t VulkanRenderPass::addSubPass(
    const vk::PipelineBindPoint& pipelineBindPoint,
    const std::uint32_t& srcSubPass, const std::uint32_t& destSubPass,
    const vk::PipelineStageFlags& srcStageMask, const vk::AccessFlags& srcAccessMask,
    const vk::PipelineStageFlags& dstStageMask, const vk::AccessFlags& dstAccessMask
)
{
    vk::SubpassDescription subpassDesc{};
    subpassDesc.pipelineBindPoint = pipelineBindPoint;
    subpassDesc.colorAttachmentCount = static_cast<std::uint32_t>( m_colorAttachmentRefs.size() );
    subpassDesc.pColorAttachments = m_colorAttachmentRefs.data();
    subpassDesc.pDepthStencilAttachment = m_depthAttachmentRefs.data();
    subpassDesc.pResolveAttachments = m_resolveAttachmentRefs.data();

    vk::SubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = srcSubPass;
    subpassDependency.dstSubpass = destSubPass;
    subpassDependency.srcStageMask = srcStageMask;
    subpassDependency.srcAccessMask = srcAccessMask;
    subpassDependency.dstStageMask = dstStageMask;
    subpassDependency.dstAccessMask = dstAccessMask;

    m_subpassDescriptions.push_back( subpassDesc );
    m_subpassDependencies.push_back( subpassDependency );    

    return static_cast<std::uint32_t>( m_subpassDescriptions.size() - 1 );
}

void VulkanRenderPass::createRenderPass()
{
    vk::RenderPassCreateInfo vkRenderPassInfo{};
    vkRenderPassInfo.attachmentCount = static_cast<std::uint32_t>( m_targetAttachments.size() );
    vkRenderPassInfo.pAttachments = m_targetAttachments.data();
    vkRenderPassInfo.subpassCount = static_cast<std::uint32_t>( m_subpassDependencies.size() );
    vkRenderPassInfo.pSubpasses = m_subpassDescriptions.data();
    vkRenderPassInfo.dependencyCount = static_cast<std::uint32_t>( m_subpassDependencies.size() );
    vkRenderPassInfo.pDependencies = m_subpassDependencies.data();

    m_vkRenderPass = m_pLogicalDevice->createRenderPass( vkRenderPassInfo );

    LOG_INFO(fmt::format("{} RenderPass Created", m_renderPassName));
}

void VulkanRenderPass::destroyRenderPass()
{
    m_pLogicalDevice->destroyRenderPass( m_vkRenderPass );
}


} // namespace vkrender