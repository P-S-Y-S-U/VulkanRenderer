#include "vkrender/VulkanGPUProgram.h"
#include "utilities/VulkanLogger.h"

#include <fstream>

namespace vkrender
{

VulkanGpuProgram::VulkanGpuProgram(const std::filesystem::path& shaderPath)
{
    populateShaderBufferFromSourceFile( shaderPath, m_shaderBuffer );
}

VulkanGpuProgram::VulkanGpuProgram(const std::vector<char>& shaderBuffer)
{}

VulkanGpuProgram::~VulkanGpuProgram()
{
    m_pDevice->destroyShaderModule( m_vkShaderModule );
}

void VulkanGpuProgram::createShader(
    vk::Device* pLogicalDevice,
    const vk::ShaderStageFlagBits& shaderStage,
    const std::string& entryPoint
)
{
    m_pDevice = pLogicalDevice;
    m_vkShaderStage = shaderStage;
    m_entryPoint = entryPoint;
    createShaderModule();
}

void VulkanGpuProgram::populateShaderBufferFromSourceFile( const std::filesystem::path& filePath, std::vector<char>& shaderBuffer )
{
    if( filePath.extension() != std::filesystem::path{ ".spv" } )
    {
        std::string errorMsg = "ILLEGAL SHADER FILE FORMAT!";
        LOG_ERROR(errorMsg);
        throw std::runtime_error(errorMsg);
    }

    std::ifstream fstream( filePath.string(), std::ios::ate | std::ios::binary );

    std::size_t fileSize = static_cast<std::size_t>( fstream.tellg() );
    shaderBuffer.resize( fileSize );

    fstream.seekg(0);
    fstream.read( shaderBuffer.data(), fileSize );

    fstream.close();
}

void VulkanGpuProgram::createShaderModule()
{
    vk::ShaderModuleCreateInfo vkShaderModuleCreateInfo{};
	vkShaderModuleCreateInfo.codeSize = m_shaderBuffer.size();
	vkShaderModuleCreateInfo.pCode = reinterpret_cast<const std::uint32_t*>( m_shaderBuffer.data() );

	m_vkShaderModule = m_pDevice->createShaderModule( vkShaderModuleCreateInfo );
}

} // namespace vkrender
