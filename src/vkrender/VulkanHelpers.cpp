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

}