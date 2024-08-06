#include "vkrender/VulkanGfxPipeline.h"
#include "utilities/VulkanLogger.h"

namespace vkrender
{

VulkanGfxPipeline::VulkanGfxPipeline( vk::Device* pLogicalDevice )
    :m_pLogicalDevice{ pLogicalDevice }
{
    m_vkViewportState.viewportCount = 1;
    m_vkViewportState.pViewports = nullptr;
    m_vkViewportState.scissorCount = 1;
    m_vkViewportState.pScissors = nullptr;
}

VulkanGfxPipeline::~VulkanGfxPipeline()
{
    destroyGfxPipeline();
}

void VulkanGfxPipeline::bindShaderStages( const ShaderStages& shaderStages )
{
    auto l_populatePipelineShaderStageCreateInfo = []( 
		vk::PipelineShaderStageCreateInfo& shaderStageCreateInfo,
		VulkanGpuProgram* pGpuProgram
	)
	{
		shaderStageCreateInfo.stage = pGpuProgram->m_vkShaderStage;
		shaderStageCreateInfo.module = pGpuProgram->m_vkShaderModule;
		shaderStageCreateInfo.pName =  pGpuProgram->m_entryPoint.c_str();
		shaderStageCreateInfo.pSpecializationInfo = nullptr;
	};

    if( shaderStages.m_vertexShader )
    {    
        m_vkShaderStages.push_back( {} );
        l_populatePipelineShaderStageCreateInfo( m_vkShaderStages[0], shaderStages.m_vertexShader );
    }
    else if( shaderStages.m_fragmentShader )
    {
        m_vkShaderStages.push_back( {} );
        l_populatePipelineShaderStageCreateInfo( m_vkShaderStages[1], shaderStages.m_fragmentShader );
    }
}

void VulkanGfxPipeline::setInputAssemblyState(
    const vk::PrimitiveTopology& primitiveTopology,
    const bool& bPrimitiveRestart
)
{
    m_vkInputAssembly.topology = primitiveTopology;
    m_vkInputAssembly.primitiveRestartEnable = static_cast<vk::Bool32>( bPrimitiveRestart );
}

void VulkanGfxPipeline::setRasterizerState(
    const vk::PolygonMode& polygonMode,
    const vk::CullModeFlagBits& cullMode, const vk::FrontFace& frontFace,
    const bool& bRasterizerDiscard,
    const bool& bDepthBias, const float& depthBiasConstFactor,
    const bool& bDepthClamp, const float& depthBiasSlopeFactor
)
{
    m_vkRasterizerState.polygonMode = polygonMode;
    m_vkRasterizerState.cullMode = cullMode;
    m_vkRasterizerState.frontFace = frontFace;
    m_vkRasterizerState.rasterizerDiscardEnable = static_cast<vk::Bool32>( bRasterizerDiscard );
    m_vkRasterizerState.depthBiasEnable = static_cast<vk::Bool32>( bDepthBias );
    m_vkRasterizerState.depthBiasConstantFactor = depthBiasConstFactor;
    m_vkRasterizerState.depthClampEnable = static_cast<vk::Bool32>( bDepthClamp );
    m_vkRasterizerState.depthBiasSlopeFactor = depthBiasSlopeFactor;
    m_vkRasterizerState.lineWidth = 1.0f;
}

void VulkanGfxPipeline::setMultisampleState(
    const vk::SampleCountFlagBits& sampleCount,
    const bool& bSampleShading, const float& minSampleShading
)
{
    m_vkMultisampleState.rasterizationSamples = sampleCount;
    m_vkMultisampleState.sampleShadingEnable = static_cast<vk::Bool32>( bSampleShading );
    m_vkMultisampleState.minSampleShading = minSampleShading;
    m_vkMultisampleState.pSampleMask = nullptr;
    m_vkMultisampleState.alphaToCoverageEnable = VK_FALSE;
    m_vkMultisampleState.alphaToOneEnable = VK_FALSE;
}

void VulkanGfxPipeline::setDepthState(
    const bool& bDepthTest, const bool& bDepthWrite,
    const vk::CompareOp& compareOp,
    const bool& bDepthBoundsTestEnable,
    const float& minDepthBounds, const float& maxDepthBounds
)
{
    m_vkDepthStencilState.depthTestEnable = static_cast<vk::Bool32>( bDepthTest );
    m_vkDepthStencilState.depthWriteEnable = static_cast<vk::Bool32>( bDepthWrite );
    m_vkDepthStencilState.depthCompareOp = compareOp;
    m_vkDepthStencilState.depthBoundsTestEnable = static_cast<vk::Bool32>( bDepthBoundsTestEnable );
    m_vkDepthStencilState.minDepthBounds = minDepthBounds;
    m_vkDepthStencilState.maxDepthBounds = maxDepthBounds;
}

void VulkanGfxPipeline::setStencilState(
    const bool& bStencilState,
    const vk::StencilOp& frontFace,
    const vk::StencilOp& backFace
)
{
    m_vkDepthStencilState.stencilTestEnable = static_cast<vk::Bool32>( bStencilState );
    m_vkDepthStencilState.front = frontFace;
    m_vkDepthStencilState.back = backFace;
}

void VulkanGfxPipeline::setColorBlendState(
    const vk::ColorComponentFlagBits& colorWriteMask,
    const bool& bColorBlend,
    const BlendOp& colorBlendOp, const BlendOp& alphaBlendOp
)
{
    m_vkColorBlendAttachment.blendEnable = VK_FALSE;
    m_vkColorBlendAttachment.srcColorBlendFactor = colorBlendOp.m_srcBlendFactor;
    m_vkColorBlendAttachment.dstColorBlendFactor = colorBlendOp.m_dstBlendFactor;
    m_vkColorBlendAttachment.colorBlendOp = colorBlendOp.m_blendOp;
    m_vkColorBlendAttachment.srcAlphaBlendFactor = alphaBlendOp.m_srcBlendFactor;
    m_vkColorBlendAttachment.dstAlphaBlendFactor = alphaBlendOp.m_dstBlendFactor;
    m_vkColorBlendAttachment.alphaBlendOp = alphaBlendOp.m_blendOp;

    m_vkColorBlendState.logicOpEnable = VK_FALSE;
    m_vkColorBlendState.logicOp = vk::LogicOp::eCopy;
    m_vkColorBlendState.attachmentCount = 1;
    m_vkColorBlendState.pAttachments = &m_vkColorBlendAttachment;
    m_vkColorBlendState.blendConstants[0] = 0.0f;
    m_vkColorBlendState.blendConstants[1] = 0.0f;
    m_vkColorBlendState.blendConstants[2] = 0.0f;
    m_vkColorBlendState.blendConstants[3] = 0.0f;
}

void VulkanGfxPipeline::setDynamicState(
    const std::vector<vk::DynamicState>& dynamicStates
)
{
    m_vkDynamicState.pDynamicStates = dynamicStates.data();
    m_vkDynamicState.dynamicStateCount = dynamicStates.size();
}

void VulkanGfxPipeline::bindDescriptors(
    const std::vector<VulkanDescriptor*>& descriptors
)
{
    for( const auto& item : descriptors )
    {
        m_vkDescriptorSetLayoutArray.push_back( item->m_vkDescriptorSetLayout );
    }
}

vk::Result VulkanGfxPipeline::createGfxPipeline(
    VulkanRenderPass* pRenderPassToUse, 
    const std::uint32_t& subPassIndexToUse
)
{
    createPipelineLayout();

    vk::GraphicsPipelineCreateInfo vkGfxPipelineCreateInfo{};
    vkGfxPipelineCreateInfo.stageCount = m_vkShaderStages.size();
    vkGfxPipelineCreateInfo.pStages = m_vkShaderStages.data();
    vkGfxPipelineCreateInfo.pVertexInputState = nullptr;
    vkGfxPipelineCreateInfo.pInputAssemblyState = &m_vkInputAssembly;
    vkGfxPipelineCreateInfo.pViewportState = &m_vkViewportState;
    vkGfxPipelineCreateInfo.pRasterizationState = &m_vkRasterizerState;
    vkGfxPipelineCreateInfo.pMultisampleState = &m_vkMultisampleState;
    vkGfxPipelineCreateInfo.pDepthStencilState = &m_vkDepthStencilState;
    vkGfxPipelineCreateInfo.pColorBlendState = &m_vkColorBlendState;
    vkGfxPipelineCreateInfo.layout = m_vkPipelineLayout;
    vkGfxPipelineCreateInfo.renderPass = pRenderPassToUse->m_vkRenderPass;
    vkGfxPipelineCreateInfo.subpass = 0;
    vkGfxPipelineCreateInfo.basePipelineHandle = nullptr;
    vkGfxPipelineCreateInfo.basePipelineIndex = -1;

    vk::ResultValue<vk::Pipeline> operationResult = m_pLogicalDevice->createGraphicsPipeline( nullptr, vkGfxPipelineCreateInfo );

	if( operationResult.result == vk::Result::eSuccess )
	{
		m_vkGfxPipeline = operationResult.value;
		LOG_INFO("Graphics Pipeline created");
	}
	else
	{
		std::string errorMsg = "Failed to create Graphics Pipeline";
		LOG_ERROR(errorMsg);
		throw std::runtime_error(errorMsg);
	}

    return operationResult.result;
}

void VulkanGfxPipeline::createPipelineLayout()
{
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.setLayoutCount = m_vkDescriptorSetLayoutArray.size();
    pipelineLayoutCreateInfo.pSetLayouts = m_vkDescriptorSetLayoutArray.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    m_vkPipelineLayout = m_pLogicalDevice->createPipelineLayout( pipelineLayoutCreateInfo );

    LOG_INFO("Pipeline Layout Created");
}

void VulkanGfxPipeline::destroyGfxPipeline()
{
    m_pLogicalDevice->destroyPipeline( m_vkGfxPipeline );
    m_pLogicalDevice->destroyPipelineLayout( m_vkPipelineLayout );
}


} // namespace vkrender
