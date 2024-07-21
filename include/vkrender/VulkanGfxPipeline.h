#ifndef VKRENDER_VULKAN_GFX_PIPELINE_H
#define VKRENDER_VULKAN_GFX_PIPELINE_H

#include "vkrender/VulkanRendererExports.hpp"
#include "vkrender/VulkanGPUProgram.h"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanGfxPipeline
{
public:
    struct ShaderStages
    {
        
    };

    VulkanGfxPipeline( vk::Device* pLogicalDevice );
    ~VulkanGfxPipeline();

private:
    vk::Device* m_pLogicalDevice;
    vk::Pipeline m_vkGfxPipeline;

    std::vector<vk::PipelineShaderStageCreateInfo> m_vkShaderStages;
    vk::PipelineInputAssemblyStateCreateInfo m_vkInputAssembly;

};

} // namespace vkrender


#endif