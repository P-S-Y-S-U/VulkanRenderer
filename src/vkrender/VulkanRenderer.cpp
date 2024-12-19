#include "vkrender/VulkanRenderer.h"
#include "vkrender/VulkanDebugMessenger.h"
#include "vkrender/VulkanHelpers.h"
#include "vkrender/VulkanTextureManager.h"

#include "utilities/VulkanLogger.h"

#include <vulkan/vulkan_wayland.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace vkrender
{

VulkanRenderer::VulkanRenderer()
{
    if (utils::VulkanRendererApiLogger::getSingletonPtr() == nullptr)
	{
		spdlog::sink_ptr consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		std::initializer_list<spdlog::sink_ptr> logSinks{
			consoleSink
		};
		utils::VulkanRendererApiLogger::createInstance(logSinks);
		utils::VulkanRendererApiLogger::getSingletonPtr()->getLogger()->set_level(spdlog::level::debug);
	}
}

VulkanRenderer::~VulkanRenderer()
{
    shutdown();
}

void VulkanRenderer::initVulkan( VulkanWindow* pVulkanWindow )
{
    createInstance();
	setupDebugMessenger();
	createSurface( pVulkanWindow );
	pickPhysicalDevice();
	createLogicalDevice();
	createCommandPool();
	createConfigCommandBuffer();

	SwapchainCreateInfo swapchainCreateInfo{};
	swapchainCreateInfo.vkPhysicalDevice = m_vkPhysicalDevice;
	swapchainCreateInfo.vkLogicalDevice = m_vkLogicalDevice;
	swapchainCreateInfo.vkSurface = m_vkSurface;
	swapchainCreateInfo.vkSampleCount = m_msaaSampleCount;
	m_pVulkanSwapchain = std::make_unique<VulkanSwapchain>( swapchainCreateInfo );
	m_pVulkanSwapchain->createSwapchain( m_pVulkanWindow->getFrameBufferSize() );

	m_pTextureManager = std::make_unique<VulkanTextureManager>( this );
}

void VulkanRenderer::shutdown()
{
	if( m_bHasExclusiveTransferQueue )
		m_vkLogicalDevice.destroyCommandPool( m_vkTransferCommandPool );
	m_vkLogicalDevice.destroyCommandPool( m_vkGraphicsCommandPool );
	LOG_DEBUG("Command Pool Destroyed");

	for( const vk::Sampler& elem : m_samplers )
		m_vkLogicalDevice.destroySampler( elem );
	LOG_DEBUG("Samplers Destroyed");

	m_pVulkanSwapchain->destroySwapchain();
	m_pVulkanSwapchain.reset();

    m_vkLogicalDevice.destroy();
	LOG_DEBUG("Logical Device Destroyed");

	m_vkInstance.destroySurfaceKHR( m_vkSurface );
	LOG_DEBUG("Vulkan Surface Destroyed");

	if( ENABLE_VALIDATION_LAYER )
	{
		VulkanDebugMessenger::destroyDebugUtilsMessengerEXT( m_vkInstance, m_vkDebugUtilsMessenger, nullptr );
		LOG_DEBUG("Debug Messenger Destroyed");
	}
	m_vkInstance.destroy();
	LOG_DEBUG("Vulkan Instance Destroyed");
}

void VulkanRenderer::prepareRendering()
{
	m_presentationRenderpass = std::make_unique<VulkanRenderPass>(
		&m_vkLogicalDevice, "Presentation Pass"
	);
	VulkanRenderTargetArray prependTargets{};
	
}

void VulkanRenderer::destroySwapchain()
{
	m_pVulkanSwapchain->destroySwapchain();
}

void VulkanRenderer::recreateSwapchain()
{
	m_vkLogicalDevice.waitIdle();

	m_pVulkanSwapchain->recreateSwapchain( m_pVulkanWindow->getFrameBufferSize() );
}

void VulkanRenderer::createBuffer(
    const vk::DeviceSize& bufferSizeInBytes,
    const vk::BufferUsageFlags& bufferUsage, const vk::SharingMode& bufferSharingMode,
    const vk::MemoryPropertyFlags& memProps,
    vk::Buffer& buffer, vk::DeviceMemory& bufferMemory
)
{
	vkrender::QueueFamilyIndices queueFamilyIndices = VulkanHelpers::findQueueFamilyIndices( 
		m_vkPhysicalDevice,
		&m_vkSurface
	);

	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.size = bufferSizeInBytes;
	bufferInfo.usage = bufferUsage;
	bufferInfo.sharingMode = bufferSharingMode;
	std::vector<uint32_t> queueFamilyToShare;
	queueFamilyToShare.emplace_back( queueFamilyIndices.m_graphicsFamily.value() );
	if( queueFamilyIndices.m_exclusiveTransferFamily.has_value() ) queueFamilyToShare.emplace_back( queueFamilyIndices.m_exclusiveTransferFamily.value() );
	bufferInfo.pQueueFamilyIndices = queueFamilyToShare.data();
	bufferInfo.queueFamilyIndexCount = queueFamilyToShare.size();

	buffer = m_vkLogicalDevice.createBuffer(
		bufferInfo
	);

	vk::MemoryRequirements memRequirements = m_vkLogicalDevice.getBufferMemoryRequirements( buffer );

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VulkanHelpers::findMemoryType( m_vkPhysicalDevice, memRequirements.memoryTypeBits, memProps );

	bufferMemory = m_vkLogicalDevice.allocateMemory( allocInfo );

	m_vkLogicalDevice.bindBufferMemory( buffer, bufferMemory, 0 );
}	

vk::Sampler* VulkanRenderer::createTexSampler(
    const vk::Filter& minFilter, const vk::Filter& magFilter,
    const vk::SamplerAddressMode& uAddrMode, const vk::SamplerAddressMode& vAddrMode, const vk::SamplerAddressMode& wAddrMode,
    const bool& bEnableAnisotropy,
    const vk::SamplerMipmapMode& mipmapMode,
    const float& minLod, const float& maxLod, const float& mipLodBias,
    const vk::BorderColor& borderColor,
    const bool& bnormalizedCoords,
    const bool& bCmpEnable, const vk::CompareOp& cmpOp
)
{
	vk::PhysicalDeviceProperties phyDeviceProp = m_vkPhysicalDevice.getProperties(); 

	vk::SamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.minFilter = minFilter;
	samplerCreateInfo.magFilter = magFilter;
	samplerCreateInfo.addressModeU = uAddrMode;
	samplerCreateInfo.addressModeV = vAddrMode;
	samplerCreateInfo.addressModeW = wAddrMode;
	samplerCreateInfo.anisotropyEnable = static_cast<vk::Bool32>( bEnableAnisotropy );
	samplerCreateInfo.maxAnisotropy = phyDeviceProp.limits.maxSamplerAnisotropy;
	samplerCreateInfo.borderColor = borderColor;
	samplerCreateInfo.unnormalizedCoordinates = static_cast<vk::Bool32>( !bnormalizedCoords );
	samplerCreateInfo.compareEnable = static_cast<vk::Bool32>( bCmpEnable );
	samplerCreateInfo.compareOp = cmpOp;
	samplerCreateInfo.mipmapMode = mipmapMode;
	samplerCreateInfo.mipLodBias = mipLodBias;
	samplerCreateInfo.minLod = minLod;
	samplerCreateInfo.maxLod = maxLod;

	m_samplers.push_back( m_vkLogicalDevice.createSampler( samplerCreateInfo ) );

	return &m_samplers[ m_samplers.size() - 1 ];
}

vk::Framebuffer VulkanRenderer::createFramebuffer(
    const VulkanRenderPass& renderPassToBind,
    const VulkanRenderTargetArray& renderTargets,
    const utils::Dimension& framebufferSize,
    const std::uint32_t& layers
)
{
	std::vector<vk::ImageView> attachments{ renderTargets.size() };
	for( auto i = 0u; i < renderTargets.size(); i++ )
	{
		attachments[i] = renderTargets[i].m_imgView;
	}

	vk::FramebufferCreateInfo fbCreateInfo{};
	fbCreateInfo.renderPass = renderPassToBind.m_vkRenderPass;
	fbCreateInfo.attachmentCount = attachments.size();
	fbCreateInfo.pAttachments = attachments.data();
	fbCreateInfo.width = framebufferSize.m_width;
	fbCreateInfo.height = framebufferSize.m_height;
	fbCreateInfo.layers = layers;

	return m_vkLogicalDevice.createFramebuffer( fbCreateInfo );
}

void VulkanRenderer::createSurface( VulkanWindow* pVulkanWindow )
{
    m_pVulkanWindow = pVulkanWindow;

#ifdef _WIN32
	vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.sType = vk::StructureType::eWin32SurfaceCreateInfoKHR;
	surfaceCreateInfo.hwnd = m_pVulkanWindow->getHandle();
	surfaceCreateInfo.hinstance = GetModuleHandle( nullptr );

	m_vkSurface = m_vkInstance.createWin32SurfaceKHR( surfaceCreateInfo );
#else
	VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.surface = m_pVulkanWindow->getHandle();
	surfaceCreateInfo.display = m_pVulkanWindow->getDisplayHandle();
	surfaceCreateInfo.pNext = nullptr;

 	VkResult surfaceCreateReslt = vkCreateWaylandSurfaceKHR( 
		reinterpret_cast<const VkInstance&>( m_vkInstance ),
		&surfaceCreateInfo,
		nullptr,
		reinterpret_cast<VkSurfaceKHR*>( &m_vkSurface )
	);

	if( surfaceCreateReslt != VK_SUCCESS )
	{
		LOG_ERROR("Failed to Create Vulkan Wayland Surface");
	}

#endif
	LOG_INFO( "Vulkan Surface Created" );
}

void VulkanRenderer::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = VulkanHelpers::findQueueFamilyIndices( m_vkPhysicalDevice, &m_vkSurface );

	vk::CommandPoolCreateInfo vkGraphicsCommandPoolInfo{};
	vkGraphicsCommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	vkGraphicsCommandPoolInfo.queueFamilyIndex = queueFamilyIndices.m_graphicsFamily.value();

	if( m_bHasExclusiveTransferQueue )
	{
		vk::CommandPoolCreateInfo vkTransferCommandPoolInfo{};
		vkTransferCommandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		vkTransferCommandPoolInfo.queueFamilyIndex = queueFamilyIndices.m_exclusiveTransferFamily.has_value() ? queueFamilyIndices.m_exclusiveTransferFamily.value() : queueFamilyIndices.m_graphicsFamily.value();

		m_vkGraphicsCommandPool = m_vkLogicalDevice.createCommandPool( vkGraphicsCommandPoolInfo );
		LOG_INFO("Graphics Command Pool created");

		m_vkTransferCommandPool = m_vkLogicalDevice.createCommandPool( vkTransferCommandPoolInfo );
		LOG_INFO("Transfer Command Pool created");
	}
	else
	{
		m_vkGraphicsCommandPool = m_vkLogicalDevice.createCommandPool( vkGraphicsCommandPoolInfo );
		LOG_INFO("Graphics Command Pool created");

		m_vkTransferCommandPool = m_vkGraphicsCommandPool;
		LOG_INFO("Using Graphics Command Pool for Transfer Operations");
	}
}

void VulkanRenderer::createConfigCommandBuffer()
{
	m_pConfigCmdBuffer = std::make_unique<VulkanImmediateCmdBuffer>(
		&m_vkLogicalDevice,
		&m_vkGraphicsQueue,
		&m_vkGraphicsCommandPool
	);
	m_pConfigCmdBuffer->allocate();
	LOG_INFO("Config Command Buffer created");
}

} // namespace vkrender