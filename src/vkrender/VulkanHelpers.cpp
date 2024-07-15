#include "vkrender/VulkanHelpers.h"
#include "utilities/VulkanLogger.h"

namespace vkrender
{

QueueFamilyIndices VulkanHelpers::findQueueFamilyIndices( const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR* pVkSurface )
{
	QueueFamilyIndices queueFamilyIndices;

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	int validQueueIndex = 0;

	for (const auto& prop : queueFamilyProperties)
	{
		if (prop.queueFlags & vk::QueueFlagBits::eGraphics )
		{
			queueFamilyIndices.m_graphicsFamily = validQueueIndex;
		}

		if( prop.queueFlags & vk::QueueFlagBits::eCompute )
		{
			queueFamilyIndices.m_computeFamily = validQueueIndex;
		}

		if( 
			prop.queueFlags & vk::QueueFlagBits::eTransfer && 
			( !( prop.queueFlags & vk::QueueFlagBits::eGraphics ) && !( prop.queueFlags & vk::QueueFlagBits::eCompute ) )
		)
		{
			queueFamilyIndices.m_exclusiveTransferFamily = validQueueIndex;
		}

		if( pVkSurface )
		{
			vk::Bool32 bPresentationSupport = physicalDevice.getSurfaceSupportKHR( validQueueIndex, *pVkSurface );
			if( bPresentationSupport )
			{
				queueFamilyIndices.m_presentFamily = validQueueIndex;					
			}
		}
		validQueueIndex++;
	}

	return queueFamilyIndices;
}

SwapChainSupportDetails VulkanHelpers::querySwapChainSupport( const vk::PhysicalDevice& vkPhysicalDevice, const vk::SurfaceKHR& vkSurface )
{
    SwapChainSupportDetails swapChainDetails;

    swapChainDetails.capabilities =	vkPhysicalDevice.getSurfaceCapabilitiesKHR( vkSurface );
    swapChainDetails.surfaceFormats = vkPhysicalDevice.getSurfaceFormatsKHR( vkSurface );
    swapChainDetails.presentModes = vkPhysicalDevice.getSurfacePresentModesKHR( vkSurface );

    return swapChainDetails;
}

vk::SampleCountFlagBits VulkanHelpers::getMaxUsableSampleCount( const vk::PhysicalDevice& vkPhysicalDevice )
{
	vk::PhysicalDeviceProperties physicalDeviceProps = vkPhysicalDevice.getProperties();

	vk::SampleCountFlags counts = physicalDeviceProps.limits.framebufferColorSampleCounts & physicalDeviceProps.limits.framebufferDepthSampleCounts;

	if( counts & vk::SampleCountFlagBits::e64 ) { return vk::SampleCountFlagBits::e64; }
	if( counts & vk::SampleCountFlagBits::e32 ) { return vk::SampleCountFlagBits::e32; }
	if( counts & vk::SampleCountFlagBits::e16 ) { return vk::SampleCountFlagBits::e16; }
	if( counts & vk::SampleCountFlagBits::e8 ) { return vk::SampleCountFlagBits::e8; }
	if( counts & vk::SampleCountFlagBits::e4 ) { return vk::SampleCountFlagBits::e4; }
	if( counts & vk::SampleCountFlagBits::e2 ) { return vk::SampleCountFlagBits::e2; }

	return vk::SampleCountFlagBits::e1;
}

std::uint32_t VulkanHelpers::findMemoryType(
	const vk::PhysicalDevice& vkPhysicalDevice,
	const std::uint32_t& typeFilter, const vk::MemoryPropertyFlags& propertyFlags
)
{
	vk::PhysicalDeviceMemoryProperties memoryProps = vkPhysicalDevice.getMemoryProperties();

	for( auto i = 0u; i < memoryProps.memoryTypeCount; i++ )
	{
		if( 
			( typeFilter & ( 1u << i ) ) &&
			( memoryProps.memoryTypes[i].propertyFlags & propertyFlags ) == propertyFlags 
		)
		{
			return i;
		}
	}

	// TODO throw exception
}

void VulkanHelpers::createImage(
	const vk::PhysicalDevice& vkPhysicalDevice, const vk::Device& vkLogicalDevice,
    const std::uint32_t& width, const std::uint32_t& height, const std::uint32_t& mipmapLevels,
	const vk::SampleCountFlagBits& numOfSamples,
    const vk::Format& format, const vk::ImageTiling& tiling,
    const vk::ImageUsageFlags& usageFlags, const vk::MemoryPropertyFlags& memPropFlags,
    vk::Image& image, vk::DeviceMemory& imageMemory
)
{
	vk::ImageCreateInfo imageCreateInfo{};
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.extent.width = static_cast<std::uint32_t>( width );
	imageCreateInfo.extent.height = static_cast<std::uint32_t>( height );
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = mipmapLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = format;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageCreateInfo.usage = usageFlags;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	imageCreateInfo.samples = numOfSamples;
	imageCreateInfo.flags = {};

	image = vkLogicalDevice.createImage( imageCreateInfo );

	vk::MemoryRequirements memRequirements = vkLogicalDevice.getImageMemoryRequirements( image );
	
	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType( vkPhysicalDevice, memRequirements.memoryTypeBits, memPropFlags );

	imageMemory = vkLogicalDevice.allocateMemory( allocInfo );

	vkLogicalDevice.bindImageMemory( image, imageMemory, 0 );
}

vk::ImageView VulkanHelpers::createImageView(
	const vk::Device& vkLogicalDevice,
	const vk::Image& image,
	const vk::Format& format,
	const vk::ImageAspectFlags& aspect,
	const std::uint32_t& mipmapLevels
)
{
	vk::ImageViewCreateInfo vkImageViewCreateInfo{};
	vkImageViewCreateInfo.image = image;
	vkImageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    vkImageViewCreateInfo.format = format;
    vk::ComponentMapping componentMap;
    componentMap.r = vk::ComponentSwizzle::eIdentity;
    componentMap.g = vk::ComponentSwizzle::eIdentity;
    componentMap.b = vk::ComponentSwizzle::eIdentity;
    componentMap.a = vk::ComponentSwizzle::eIdentity;
    vkImageViewCreateInfo.components = componentMap;
    vk::ImageSubresourceRange subResourceRange;
    subResourceRange.aspectMask = aspect;
    subResourceRange.baseMipLevel = 0;
    subResourceRange.levelCount = mipmapLevels;
    subResourceRange.baseArrayLayer = 0;
    subResourceRange.layerCount = 1;
    vkImageViewCreateInfo.subresourceRange = subResourceRange;

	return vkLogicalDevice.createImageView( vkImageViewCreateInfo );
}

void VulkanHelpers::transitionImageLayout(
    VulkanCmdBuffer* pCmdBuffer,
    const vk::Image& image, const vk::Format& format, 
    const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout,
    const std::uint32_t& mipmapLevels
)
{
	vk::CommandBuffer* pVkCmdBuf = pCmdBuffer->beginCmdBuffer();

	vk::AccessFlags srcAccessMask;
	vk::AccessFlags dstAccessMask;

	vk::PipelineStageFlags srcStage;
	vk::PipelineStageFlags dstStage;
	
	vk::ImageAspectFlags aspectMask;

	if( newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal )
	{
		aspectMask = vk::ImageAspectFlagBits::eDepth;

		if( hasStencilComponent(format) )
			aspectMask |= vk::ImageAspectFlagBits::eStencil;
	}
	else
	{
		aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	if( oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal )
	{
		srcAccessMask = {};
		dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if( oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal )
	{
		srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		dstAccessMask = vk::AccessFlagBits::eShaderRead;

		srcStage = vk::PipelineStageFlagBits::eTransfer;
		dstStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if( oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal )
	{
		srcAccessMask = {};
		dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else 
	{
		std::string errorMsg = "unsupported image layout transition!";
		LOG_ERROR(errorMsg);
		throw  std::invalid_argument(errorMsg);
	}

	vk::ImageMemoryBarrier imgBarrier{};
	imgBarrier.oldLayout = oldLayout;
	imgBarrier.newLayout = newLayout;
	imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgBarrier.image = image;
	imgBarrier.subresourceRange.aspectMask = aspectMask;
	imgBarrier.subresourceRange.baseMipLevel = 0;
	imgBarrier.subresourceRange.levelCount = mipmapLevels;
	imgBarrier.subresourceRange.baseArrayLayer = 0;
	imgBarrier.subresourceRange.layerCount = 1;
	imgBarrier.srcAccessMask = srcAccessMask;
	imgBarrier.dstAccessMask = dstAccessMask;

	pVkCmdBuf->pipelineBarrier( 
		srcStage, dstStage,
		{},
		0, nullptr,
		0, nullptr,
		1, &imgBarrier
	);

	pCmdBuffer->endCmdBuffer();
}

vk::Format VulkanHelpers::findSupportedImgFormat(
	const vk::PhysicalDevice& vkPhysicalDevice,
    const std::initializer_list<vk::Format>& candidates,
    const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& features
)
{
	for( const vk::Format& format : candidates )
	{
		vk::FormatProperties prop = vkPhysicalDevice.getFormatProperties( format );

		if( tiling == vk::ImageTiling::eLinear && (prop.linearTilingFeatures & features) == features )
		{
			return format;
		}
		else if ( tiling == vk::ImageTiling::eOptimal && (prop.optimalTilingFeatures & features) == features )
		{
			return format;
		}
	}

	std::string errorMsg{ "failed to find supported format"};
	LOG_ERROR(errorMsg);
	throw std::runtime_error(errorMsg);
}

bool VulkanHelpers::hasStencilComponent( const vk::Format& format )
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

}