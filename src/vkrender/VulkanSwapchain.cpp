#include "vkrender/VulkanSwapchain.h"
#include "vkrender/VulkanQueueFamily.hpp"
#include "vkrender/VulkanRenderTarget.h"
#include "vkrender/VulkanHelpers.h"
#include "utilities/VulkanLogger.h"

namespace vkrender
{

VulkanSwapchain::VulkanSwapchain( const SwapchainCreateInfo& swapchainCreateInfo )
    :m_vkPhysicalDevice{ swapchainCreateInfo.vkPhysicalDevice }
    ,m_vkLogicalDevice{ swapchainCreateInfo.vkLogicalDevice }
    ,m_vkSurface{ swapchainCreateInfo.vkSurface }
    ,m_vkSampleCount{ swapchainCreateInfo.vkSampleCount }
{}

VulkanSwapchain::~VulkanSwapchain()
{}

void VulkanSwapchain::createSwapchain( const utils::Dimension& framebufferDimension )
{
    m_framebufferSize = framebufferDimension;

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
		m_vkSwapchainImageViews[i] = VulkanHelpers::createImageView(
            m_vkLogicalDevice,
            m_vkSwapchainImages[i], m_vkSwapchainImageFormat,
            vk::ImageAspectFlagBits::eColor,
            1
        );
	}
	m_vkSwapchainImageViews.shrink_to_fit();

	LOG_INFO("Swapchain ImageViews created");
}

void VulkanSwapchain::recreateSwapchain( 
    const utils::Dimension& framebufferDimension,
    const VulkanRenderPass& passToBind, const VulkanRenderTargetArray& prependRTs
)
{
	destroySwapchain();
	
	createSwapchain( framebufferDimension );
	createSwapchainImageViews();
    createSwapchainFramebuffers( passToBind, prependRTs );
}

void VulkanSwapchain::createSwapchainFramebuffers( const VulkanRenderPass& passToBind, const VulkanRenderTargetArray& prependRTs )
{
    m_swapchainFramebuffers.resize( m_vkSwapchainImages.size() );

    for( auto i = 0u; i < m_swapchainFramebuffers.size(); i++ )
    {
        VulkanRenderTargetArray targets{ prependRTs };
        VulkanRenderTarget& swapchainRT = targets.emplace_back(
            m_vkSwapchainImageViews[i], m_vkSwapchainImageFormat,
            m_vkSampleCount,
            !prependRTs.empty()
        );
        prepareSwapchainAttachment( swapchainRT );
        m_swapchainFramebuffers[i] = createFramebuffer(
            passToBind,
            targets,
            m_framebufferSize,
            1
        );
    }
}

void VulkanSwapchain::prepareSwapchainAttachment( VulkanRenderTarget& swapchainRT )
{
    swapchainRT.setTargetLayout(
        vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR,
        vk::ImageLayout::eColorAttachmentOptimal
    );
    swapchainRT.setTargetSemantics(
        vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore
    );
}

void VulkanSwapchain::destroySwapchain()
{
	for( auto& vkImageView : m_vkSwapchainImageViews )
	{
		m_vkLogicalDevice.destroyImageView( vkImageView );
	}
	// m_vkSwapchainFramebuffers.clear();
	m_vkSwapchainImageViews.clear();

    if( m_vkSwapchain != vk::SwapchainKHR{} )
	    m_vkLogicalDevice.destroySwapchainKHR( m_vkSwapchain );
}

vk::SurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat( const SwapChainSupportDetails& swapChainSupportDetails )
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

vk::PresentModeKHR VulkanSwapchain::chooseSwapPresentMode( const SwapChainSupportDetails& swapChainSupportDetails )
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

vk::Extent2D VulkanSwapchain::chooseSwapExtent( 
    const SwapChainSupportDetails& swapChainSupportDetials, 
    const utils::Dimension& framebufferSize
)
{
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities = swapChainSupportDetials.capabilities;

    if( surfaceCapabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max() )
    {
        return surfaceCapabilities.currentExtent;
    }
    else
    {
        vk::Extent2D actualExtent{
            framebufferSize.m_width, framebufferSize.m_height
        };

        actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width );
        actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height );

        return actualExtent;
    }
}
 
std::uint32_t VulkanSwapchain::chooseImageCount( const SwapChainSupportDetails& swapChainSupportDetails )
{
    std::uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1; // always ask for minImageCount + 1

    if( swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.maxImageCount )
    {
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    return imageCount;
}

vk::Format VulkanSwapchain::findDepthFormat()
{
    return VulkanHelpers::findSupportedImgFormat(
        m_vkPhysicalDevice,
		{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment
	);
}

vk::Framebuffer VulkanSwapchain::createFramebuffer(
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

} // namespace vkrender
