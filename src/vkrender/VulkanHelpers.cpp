#include "vkrender/VulkanHelpers.h"

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

}