#ifndef VKRENDER_VULKAN_GFX_PIPELINE_H
#define VKRENDER_VULKAN_GFX_PIPELINE_H

#include "vkrender/VulkanRendererExports.hpp"
#include "vkrender/VulkanGPUProgram.h"
#include "vkrender/VulkanRenderPass.h"
#include "vkrender/VulkanDescriptor.h"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanGfxPipeline
{
public:
    struct ShaderStages
    {
        VulkanGpuProgram* m_vertexShader;
        VulkanGpuProgram* m_fragmentShader;
    };
    struct BlendOp
    {
        vk::BlendOp m_blendOp;
        vk::BlendFactor m_srcBlendFactor;
        vk::BlendFactor m_dstBlendFactor;
    };

    VulkanGfxPipeline( vk::Device* pLogicalDevice );
    ~VulkanGfxPipeline();

    void bindShaderStages( const ShaderStages& shaderStages );
    void setInputAssemblyState(
        const vk::PrimitiveTopology& primitiveTopology,
        const bool& bPrimitiveRestart = false
    );
    void setRasterizerState(
        const vk::PolygonMode& polygonMode = vk::PolygonMode::eFill,
        const vk::CullModeFlagBits& culMode = vk::CullModeFlagBits::eBack, const vk::FrontFace& frontFace = vk::FrontFace::eCounterClockwise,
        const bool& bRasterizerDiscard = false,
        const bool& bDepthBias = false, const float& depthBiasConstFactor = 0.0f,
        const bool& bDepthClamp = false, const float& depthBiasSlopeFactor = 0.0f
    );
    void setMultisampleState(
        const vk::SampleCountFlagBits& sampleCount = vk::SampleCountFlagBits::e1,
        const bool& bSampleShading = false, const float& minSampleShading = 1.0f
    );
    void setDepthState(
        const bool& bDepthTest, const bool& bDepthWrite,
        const vk::CompareOp& = vk::CompareOp::eLess,
        const bool& bDepthBoundsTestEnable = false,
        const float& minDepthBounds = 0.0f, const float& maxDepthBounds = 1.0f
    );
    void setStencilState(
        const bool& bStencilState,
        const vk::StencilOp& frontFace = vk::StencilOp::eKeep,
        const vk::StencilOp& backFace = vk::StencilOp::eKeep
    );
    void setColorBlendState(
        const vk::ColorComponentFlagBits& colorWriteMask,
        const bool& bColorBlend = false,
        const BlendOp& colorBlendOp = { vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero },
        const BlendOp& alphaBlendOp = { vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero }
    );

    void setDynamicState(
        const std::vector<vk::DynamicState>& dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor }
    );

    void bindDescriptors(
        const std::vector<VulkanDescriptor*>& descriptors 
    );

    vk::Result createGfxPipeline(
        VulkanRenderPass* pRenderPassToUse, 
        const std::uint32_t& subPassIndexToUse
    );

private:
    vk::Device* m_pLogicalDevice;
    vk::PipelineLayout m_vkPipelineLayout;
    vk::Pipeline m_vkGfxPipeline;

    std::vector<vk::PipelineShaderStageCreateInfo> m_vkShaderStages;
    vk::PipelineVertexInputStateCreateInfo m_vkVertexInput;
    vk::PipelineInputAssemblyStateCreateInfo m_vkInputAssembly;
    vk::PipelineViewportStateCreateInfo m_vkViewportState;
    vk::PipelineRasterizationStateCreateInfo m_vkRasterizerState;
    vk::PipelineMultisampleStateCreateInfo m_vkMultisampleState;
    vk::PipelineDepthStencilStateCreateInfo m_vkDepthStencilState;
    vk::PipelineColorBlendAttachmentState m_vkColorBlendAttachment;
    vk::PipelineColorBlendStateCreateInfo m_vkColorBlendState;
    vk::PipelineDynamicStateCreateInfo m_vkDynamicState;

    std::vector<vk::DescriptorSetLayout> m_vkDescriptorSetLayoutArray;

    void createPipelineLayout();
    void destroyGfxPipeline();
};

} // namespace vkrender


#endif