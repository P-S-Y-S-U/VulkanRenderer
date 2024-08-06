#ifndef VKRENDER_VULKAN_GPU_PROGRAM_H
#define VKRENDER_VULKAN_GPU_PROGRAM_H

#include "vkrender/VulkanRendererExports.hpp"

#include <filesystem>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanGpuProgram
{
public:
    VulkanGpuProgram(const std::filesystem::path& shaderPath);
    VulkanGpuProgram(const std::vector<char>& shaderBuffer);
    ~VulkanGpuProgram();

    void createShader(
        vk::Device* pLogicalDevice,
        const vk::ShaderStageFlagBits& shaderStage,
        const std::string& entryPoint
    );

private:
    vk::Device* m_pDevice;
    vk::ShaderModule m_vkShaderModule;
    vk::ShaderStageFlagBits m_vkShaderStage;    
    std::string m_entryPoint;

    std::vector<char> m_shaderBuffer;

    void populateShaderBufferFromSourceFile( const std::filesystem::path& filePath, std::vector<char>& shaderBuffer );
    void createShaderModule();

    friend class VulkanGfxPipeline;
};

} // namespace vkrender

#endif 