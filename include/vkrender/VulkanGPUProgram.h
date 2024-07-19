#ifndef VKRENDER_VULKAN_GPU_PROGRAM_H
#define VKRENDER_VULKAN_GPU_PROGRAM_H

#include <filesystem>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VulkanGpuProgram
{
public:
    VulkanGpuProgram(const std::filesystem::path& shaderPath);
    VulkanGpuProgram(const std::vector<char>& shaderBuffer);
    ~VulkanGpuProgram();

    void createShader(
        const vk::ShaderStageFlagBits& shaderStage,
        
    )
private:

    void populateShaderBufferFromSourceFile( const std::filesystem::path& filePath, std::vector<char>& shaderBuffer );
    createShaderModule()
};

} // namespace vkrender

#endif 