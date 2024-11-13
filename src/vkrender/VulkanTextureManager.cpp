#include "vkrender/VulkanTextureManager.h"
#include "vkrender/VulkanTexture.h"
#include "vkrender/VulkanCommandBuffer.h"
#include "vkrender/VulkanHelpers.h"
#include "utilities/VulkanLogger.h"

namespace vkrender
{

VulkanTextureManager::VulkanTextureManager( vkrender::VulkanRenderer* pVkRenderer )
    :m_pVkRenderer{ pVkRenderer }
{}

VulkanTextureManager::~VulkanTextureManager()
{}

VulkanTexture* VulkanTextureManager::createTexture(
    const utils::Dimension& texDimension, const std::uint32_t& miplevels,
    const vk::Format& imgFormat, const vk::ImageTiling& imgTiling,
    const vk::ImageUsageFlags& imgUsageFlags, const vk::MemoryPropertyFlags& memoryPropertyFlags,
    const vk::SampleCountFlagBits& imgSampleCountFlags,
    const vk::ImageAspectFlags& imgAspect
)
{
    VulkanTexture* pTexture = &m_textureArray.emplace_back(
        this, texDimension, miplevels,
        imgFormat, imgTiling,
        imgUsageFlags, memoryPropertyFlags,
        imgSampleCountFlags, imgAspect
    );

    pTexture->createImage();
	pTexture->createImageView();
    return pTexture;
}

VulkanTexture* VulkanTextureManager::createTextureAndUploadBuffer(
    const utils::Image& img,
    const vk::Format& imgFormat, const vk::ImageTiling& imgTiling,
    const vk::ImageUsageFlags& imgUsageFlags, const vk::MemoryPropertyFlags& memoryPropertyFlags,
    const vk::SampleCountFlagBits& imgSampleCountFlags,
    const vk::ImageAspectFlags& imgAspect
)
{
    VulkanTexture* pTexture = createTexture(
        img.dimension(), img.miplevels(),
        imgFormat, imgTiling,
        imgUsageFlags, memoryPropertyFlags,
        imgSampleCountFlags, imgAspect
    );

    transitionImageLayout(
        pTexture,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal
    );

    transferImgBufferToTexture(
        img, pTexture
    );

	generateMipmaps( pTexture );

	return pTexture;
}

void VulkanTextureManager::transitionImageLayout(
    VulkanTexture* pTexture,
    const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout
)
{
    m_pVkRenderer->m_pConfigCmdBuffer->beginCmdBuffer();

    vk::AccessFlags srcAccessMask;
	vk::AccessFlags dstAccessMask;

	vk::PipelineStageFlags srcStage;
	vk::PipelineStageFlags dstStage;
	
	vk::ImageAspectFlags aspectMask;

	if( newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal )
	{
		aspectMask = vk::ImageAspectFlagBits::eDepth;

		if( VulkanHelpers::hasStencilComponent(pTexture->m_vkImgFormat) )
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
	imgBarrier.image = pTexture->m_vkImage;
	imgBarrier.subresourceRange.aspectMask = aspectMask;
	imgBarrier.subresourceRange.baseMipLevel = 0;
	imgBarrier.subresourceRange.levelCount = pTexture->m_miplevels;
	imgBarrier.subresourceRange.baseArrayLayer = 0;
	imgBarrier.subresourceRange.layerCount = 1;
	imgBarrier.srcAccessMask = srcAccessMask;
	imgBarrier.dstAccessMask = dstAccessMask;

	m_pVkRenderer->m_pConfigCmdBuffer->handle()->pipelineBarrier( 
		srcStage, dstStage,
		{},
		0, nullptr,
		0, nullptr,
		1, &imgBarrier
	);

    m_pVkRenderer->m_pConfigCmdBuffer->endCmdBuffer();
}

void VulkanTextureManager::transferImgBufferToTexture(
    const utils::Image& img, VulkanTexture* pTexture
)
{
    vk::DeviceSize imageSizeInBytes = static_cast<vk::DeviceSize>( img.sizeInBytes() );

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    vk::SharingMode stagingBufferMemorySharingMode = m_pVkRenderer->m_bHasExclusiveTransferQueue ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;
    m_pVkRenderer->createBuffer(
        imageSizeInBytes,
        vk::BufferUsageFlagBits::eTransferSrc, stagingBufferMemorySharingMode,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory
    );

    void* bufferMapped = getDevice()->mapMemory( stagingBufferMemory, 0, imageSizeInBytes );
    std::memcpy(bufferMapped, img.buffer(), img.sizeInBytes() );
    getDevice()->unmapMemory( stagingBufferMemory );

	utils::Uptr<VulkanCmdBuffer> cmdBuf = std::make_unique<VulkanTemporaryCmdBuffer>( 
		getDevice(),
		&m_pVkRenderer->m_vkTransferQueue,
		&m_pVkRenderer->m_vkTransferCommandPool
	);
	cmdBuf->allocate();

	vk::BufferImageCopy copyRegion{};
	copyRegion.bufferOffset = 0;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;
	copyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	copyRegion.imageSubresource.mipLevel = 0;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageSubresource.layerCount = 1;
	copyRegion.imageOffset = vk::Offset3D{ 0, 0, 0 };
	copyRegion.imageExtent = vk::Extent3D{ img.dimension().m_width, img.dimension().m_height, 1 };

	cmdBuf->beginCmdBuffer();

	cmdBuf->handle()->copyBufferToImage(
		stagingBuffer, pTexture->m_vkImage,
		vk::ImageLayout::eTransferDstOptimal,
		1, &copyRegion
	);

	cmdBuf->endCmdBuffer();

	getDevice()->destroyBuffer( stagingBuffer, nullptr );
	getDevice()->freeMemory( stagingBufferMemory, nullptr );
}

void VulkanTextureManager::generateMipmaps( VulkanTexture* pTexture )
{
	vk::FormatProperties formatProps = m_pVkRenderer->m_vkPhysicalDevice.getFormatProperties( pTexture->m_vkImgFormat );

	if( !( formatProps.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear ) )
	{
		std::string errormsg = "texture image format does not support linear blitting";
		LOG_ERROR(errormsg);
		throw std::runtime_error(errormsg);
	}

	std::int32_t mipImgWidth = static_cast<std::int32_t>( pTexture->m_texDimension.m_width );
	std::int32_t mipImgHeight = static_cast<std::int32_t>( pTexture->m_texDimension.m_height );

	utils::Uptr<VulkanCmdBuffer> cmdBuf = std::make_unique<VulkanTemporaryCmdBuffer>( 
		getDevice(),
		&m_pVkRenderer->m_vkGraphicsQueue,
		&m_pVkRenderer->m_vkGraphicsCommandPool
	);
	cmdBuf->allocate();

	vk::ImageMemoryBarrier imgBarrier{};
	imgBarrier.image = pTexture->m_vkImage;
	imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imgBarrier.subresourceRange.baseArrayLayer = 0;
	imgBarrier.subresourceRange.layerCount = 1;
	imgBarrier.subresourceRange.levelCount = 1;

	cmdBuf->beginCmdBuffer();

	for( int32_t i = 1; i < pTexture->m_miplevels; i++ )
	{
		// Transition the baseMipLevel to Transfer source first
		imgBarrier.subresourceRange.baseMipLevel = i - 1;
		imgBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		imgBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
		imgBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		imgBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

		cmdBuf->handle()->pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {},
			0, nullptr, 
			0, nullptr,
			1, &imgBarrier
		);

		// Blit Image
		vk::ImageBlit imgBlit{};
		imgBlit.srcOffsets[0] = vk::Offset3D{ 0, 0, 0 };
		imgBlit.srcOffsets[1] = vk::Offset3D{ mipImgWidth, mipImgHeight, 1 };
		imgBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		imgBlit.srcSubresource.mipLevel = i - 1;
		imgBlit.srcSubresource.baseArrayLayer = 0;
		imgBlit.srcSubresource.layerCount = 1;
		imgBlit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
		imgBlit.dstOffsets[1] = vk::Offset3D{ 
			mipImgWidth > 1 ?  mipImgWidth / 2 : 1,
			mipImgHeight > 1 ? mipImgHeight / 2 : 1,
			1
		};
		imgBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		imgBlit.dstSubresource.mipLevel = i;
		imgBlit.dstSubresource.baseArrayLayer = 0;
		imgBlit.dstSubresource.layerCount = 1;

		vk::ArrayProxy<vk::ImageBlit> imgBlitArray{ imgBlit };
		
		cmdBuf->handle()->blitImage(
			pTexture->m_vkImage, vk::ImageLayout::eTransferSrcOptimal,
			pTexture->m_vkImage, vk::ImageLayout::eTransferDstOptimal,
			imgBlitArray,
			vk::Filter::eLinear
		);
		
		// Transition the baseMipLevel to Shader
		imgBarrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
		imgBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		imgBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		imgBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;		

		cmdBuf->handle()->pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {},
			0, nullptr, 
			0, nullptr,
			1, &imgBarrier
		);

		if( mipImgWidth > 1 ) mipImgWidth /= 2;
		if( mipImgHeight > 1 ) mipImgHeight /= 2;
	}

	// Transition the finalMiplevel to Shader
	imgBarrier.subresourceRange.baseMipLevel = pTexture->m_miplevels - 1;
	imgBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	imgBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	imgBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
	imgBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

	cmdBuf->handle()->pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {},
		0, nullptr, 
		0, nullptr,
		1, &imgBarrier
	);

	cmdBuf->endCmdBuffer();
}

} // namespace vkrender
