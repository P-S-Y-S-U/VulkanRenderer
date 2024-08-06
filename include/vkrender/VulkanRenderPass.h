#ifndef VKRENDER_VULKAN_RENDER_PASS_H
#define VKRENDER_VULKAN_RENDER_PASS_H

#include "vkrender/VulkanRendererExports.hpp"
#include "vkrender/VulkanRenderTarget.h"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanRenderPass
{
public:
    using VulkanRenderTargetArray = std::vector<VulkanRenderTarget>;

    VulkanRenderPass( vk::Device* pLogicalDevice, const std::string& renderPassName = "Color Pass" );
    ~VulkanRenderPass();
    
    // TODO Need to revisit this attachment & subpass API
    void prepareTargetAttachments( const VulkanRenderTargetArray& attachments );
    std::uint32_t addSubPass(
        const vk::PipelineBindPoint& pipelineBindPoint,
        const std::uint32_t& srcSubPass, const std::uint32_t& destSubPass,
        const vk::PipelineStageFlags& srcStageMask, const vk::AccessFlags& srcAccessMask,
        const vk::PipelineStageFlags& destStageMask, const vk::AccessFlags& dstAccessMask
    );
    void createRenderPass();
private:
    vk::Device* m_pLogicalDevice;
    vk::RenderPass m_vkRenderPass;

    std::vector<vk::AttachmentDescription> m_targetAttachments;

    std::vector<vk::SubpassDescription> m_subpassDescriptions;
    std::vector<vk::SubpassDependency> m_subpassDependencies;

    std::vector<vk::AttachmentReference> m_colorAttachmentRefs;
    std::vector<vk::AttachmentReference> m_depthAttachmentRefs;
    std::vector<vk::AttachmentReference> m_resolveAttachmentRefs;

    std::string m_renderPassName;

    void destroyRenderPass();

    friend class VulkanRenderer;
    friend class VulkanGfxPipeline;
};

} // namespace vkrender

#endif