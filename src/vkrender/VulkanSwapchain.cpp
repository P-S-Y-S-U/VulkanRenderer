#include "vkrender/VulkanSwapchain.h"
#include "vkrender/VulkanQueueFamily.hpp"
#include "vkrender/VulkanHelpers.h"
#include "utilities/VulkanLogger.h"

namespace vkrender
{

VulkanSwapchain::VulkanSwapchain( const SwapchainCreateInfo& swapchainCreateInfo )
    :m_vkPhysicalDevice{ swapchainCreateInfo.vkPhysicalDevice }
    ,m_vkLogicalDevice{ swapchainCreateInfo.vkLogicalDevice }
    ,m_vkSurface{ swapchainCreateInfo.vkSurface }
    ,m_vkSampleCount{ swapchainCreateInfo.vkSampleCount }
    ,m_framebufferSize{ swapchainCreateInfo.framebufferSize }
{}

VulkanSwapchain::~VulkanSwapchain()
{
    destroySwapchain();
}

void VulkanSwapchain::createSwapchain()
{
	const SwapChainSupportDetails& swapChainSupportDetails = VulkanHelpers::querySwapChainSupport(m_vkPhysicalDevice, m_vkSurface);

    const vk::SurfaceCapabilitiesKHR& capabilities = swapChainSupportDetails.capabilities;
    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat( swapChainSupportDetails );
    std::uint32_t imageCount = chooseImageCount( swapChainSupportDetails );
    vk::Extent2D imageExtent = chooseSwapExtent( swapChainSupportDetails, m_framebufferSize );
    vk::PresentModeKHR presentMode = chooseSwapPresentMode( swapChainSupportDetails );

	QueueFamilyIndices queueFamilyIndices = VulkanHelpers::findQueueFamilyIndices( m_vkPhysicalDevice, &m_vkSurface );
	std::vector<std::uint32_t> queueFamilyContainer;
    if( queueFamilyIndices.m_graphicsFamily.value() != queueFamilyIndices.m_presentFamily.value() )
    {
        m_vkSwapchainImageSharingMode = vk::SharingMode::eConcurrent;
        queueFamilyContainer.push_back( queueFamilyIndices.m_graphicsFamily.value() );
        queueFamilyContainer.push_back( queueFamilyIndices.m_presentFamily.value() );
		LOG_INFO("Different Queue Familiy for Graphics and Presentation using Concurrent mode for swapchain");
    }
    else
    {
        m_vkSwapchainImageSharingMode = vk::SharingMode::eExclusive;
		LOG_INFO("Different Queue Familiy for Graphics and Presentation using Exclusive mode for swapchain");
    }
	queueFamilyContainer.shrink_to_fit();

	vk::SwapchainCreateInfoKHR vkSwapChainCreateInfo{};
	vkSwapChainCreateInfo.surface = m_vkSurface;
	vkSwapChainCreateInfo.imageFormat = surfaceFormat.format;
    vkSwapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    vkSwapChainCreateInfo.minImageCount = imageCount;
    vkSwapChainCreateInfo.imageExtent = imageExtent;
    vkSwapChainCreateInfo.imageArrayLayers = 1;
    vkSwapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    vkSwapChainCreateInfo.imageSharingMode = m_vkSwapchainImageSharingMode; // TODO subject to break
    vkSwapChainCreateInfo.queueFamilyIndexCount = queueFamilyContainer.size();
    vkSwapChainCreateInfo.pQueueFamilyIndices = queueFamilyContainer.data();
    vkSwapChainCreateInfo.preTransform = capabilities.currentTransform;
    vkSwapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    vkSwapChainCreateInfo.presentMode = presentMode;
    vkSwapChainCreateInfo.clipped = VK_TRUE;
    vkSwapChainCreateInfo.oldSwapchain = nullptr;
	
	m_vkSwapchain = m_vkLogicalDevice.createSwapchainKHR( vkSwapChainCreateInfo );
	m_vkSwapchainImages = m_vkLogicalDevice.getSwapchainImagesKHR( m_vkSwapchain );
	m_vkSwapchainImageFormat = surfaceFormat.format;
	m_vkSwapchainExtent = imageExtent;

	LOG_INFO("Swapchain Created");
}

void VulkanSwapchain::createSwapchainImageViews()
{
	m_vkSwapchainImageViews.resize( m_vkSwapchainImages.size() );

	for( auto i = 0u; i < m_vkSwapchainImages.size(); i++ )
	{
		m_vkSwapchainImageViews[i] = createImageView( 
            m_vkSwapchainImages[i],
            m_vkSwapchainImageFormat,
            vk::ImageAspectFlagBits::eColor,
            1
        );
	}
	m_vkSwapchainImageViews.shrink_to_fit();

	LOG_INFO("Swapchain ImageViews created");
}

void VulkanSwapchain::recreateSwapchain( const utils::Dimension& framebufferDimension )
{
	m_framebufferSize = framebufferDimension;

	m_vkLogicalDevice.waitIdle();

	destroySwapchain();
	
	createSwapchain();
	createSwapchainImageViews();
	createColorResources();
	createDepthResources();
	createFramebuffers();	
}

void VulkanSwapchain::destroySwapchain()
{
	m_vkLogicalDevice.destroyImageView( m_vkColorImageView );
	m_vkLogicalDevice.destroyImage( m_vkColorImage );
	m_vkLogicalDevice.freeMemory( m_vkColorImageMemory );

	m_vkLogicalDevice.destroyImageView( m_vkDepthImageView );
	m_vkLogicalDevice.destroyImage( m_vkDepthImage );
	m_vkLogicalDevice.freeMemory( m_vkDepthImageMemory );

	for( auto& vkFramebuffer : m_vkSwapchainFramebuffers )
	{
		m_vkLogicalDevice.destroyFramebuffer( vkFramebuffer );
	}

	for( auto& vkImageView : m_vkSwapchainImageViews )
	{
		m_vkLogicalDevice.destroyImageView( vkImageView );
	}
	m_vkSwapchainFramebuffers.clear();
	m_vkSwapchainImageViews.clear();

	m_vkLogicalDevice.destroySwapchainKHR( m_vkSwapchain );
}

void VulkanSwapchain::createColorResources()
{
	createImage(
		m_vkSwapchainExtent.width, m_vkSwapchainExtent.height, 1,
		m_vkSampleCount,
		m_vkSwapchainImageFormat, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		m_vkColorImage, m_vkColorImageMemory
	);

	m_vkColorImageView = createImageView( 
        m_vkColorImage, 
        m_vkSwapchainImageFormat, 
        vk::ImageAspectFlagBits::eColor, 
        1 
    );
}

void VulkanSwapchain::createDepthResources()
{
	m_vkDepthImageFormat = findDepthFormat();

	createImage(
		m_vkSwapchainExtent.width, m_vkSwapchainExtent.height, 1, m_vkSampleCount,
		m_vkDepthImageFormat, vk::ImageTiling::eOptimal, 
		vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal,
		m_vkDepthImage, m_vkDepthImageMemory
	);
	m_vkDepthImageView = createImageView( 
        m_vkDepthImage,
        m_vkDepthImageFormat,
        vk::ImageAspectFlagBits::eDepth,
        1
    );

	transitionImageLayout( 
		m_vkDepthImage, m_vkDepthImageFormat, 
		vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal,
		1
	);

	LOG_INFO("Depth Resources Created");
}

void VulkanSwapchain::createFramebuffers()
{
	m_vkSwapchainFramebuffers.resize( m_vkSwapchainImageViews.size() );

	for( size_t i = 0; i < m_vkSwapchainFramebuffers.size(); i++ )
	{
		std::array<vk::ImageView, 3> attachments = {
			m_vkColorImageView,
			m_vkDepthImageView,
			m_vkSwapchainImageViews[i]
		};

		vk::FramebufferCreateInfo vkFrameBufferInfo{};
		vkFrameBufferInfo.renderPass = m_vkRenderPass;
		vkFrameBufferInfo.attachmentCount = static_cast<std::uint32_t>( attachments.size() );
		vkFrameBufferInfo.pAttachments = attachments.data();
		vkFrameBufferInfo.width = m_vkSwapchainExtent.width;
		vkFrameBufferInfo.height = m_vkSwapchainExtent.height;
		vkFrameBufferInfo.layers = 1;

		vk::Framebuffer vkFrameBuffer = m_vkLogicalDevice.createFramebuffer( vkFrameBufferInfo );

		m_vkSwapchainFramebuffers[i] = vkFrameBuffer;
	}
	LOG_INFO( fmt::format("{} Framebuffer created", m_vkSwapchainFramebuffers.size() ) );
}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat( const vkrender::SwapChainSupportDetails& swapChainSupportDetails )
{
    for( const auto& availableFormat : swapChainSupportDetails.surfaceFormats )
    {
        if( availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
        {
            return availableFormat;
        }
    }
    return swapChainSupportDetails.surfaceFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode( const vkrender::SwapChainSupportDetails& swapChainSupportDetails )
{
    for( const auto& availablePresentMode : swapChainSupportDetails.presentModes )
    {
        if( availablePresentMode == vk::PresentModeKHR::eMailbox )
        {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapExtent( const vkrender::SwapChainSupportDetails& swapChainSupportDetials, const vkrender::Window& window )
{
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities = swapChainSupportDetials.capabilities;

    if( surfaceCapabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max() )
    {
        return surfaceCapabilities.currentExtent;
    }
    else
    {
        const auto& [width, height] = window.getFrameBufferSize();

        vk::Extent2D actualExtent{
            width, height
        };

        actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width );
        actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height );

        return actualExtent;
    }
}
 
std::uint32_t chooseImageCount( const vkrender::SwapChainSupportDetails& swapChainSupportDetails )
{
    std::uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1; // always ask for minImageCount + 1

    if( swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.maxImageCount )
    {
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    return imageCount;
}


} // namespace vkrender
