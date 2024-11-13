#include "vkrender/VulkanTexture.h"
#include "vkrender/VulkanTextureManager.h"
#include "vkrender/VulkanHelpers.h"

namespace vkrender
{

VulkanTexture::VulkanTexture(
    VulkanTextureManager* pTextureManager,
    const utils::Dimension& texDimension, const std::uint32_t& miplevels,
    const vk::Format& imgFormat, const vk::ImageTiling& imgTiling,
    const vk::ImageUsageFlags& imgUsageFlags, const vk::MemoryPropertyFlags& memoryPropertyFlags,
    const vk::SampleCountFlagBits& imgSampleCountFlags,
    const vk::ImageAspectFlags& imgAspect
)
    :m_pTextureManager{ pTextureManager }
    ,m_texDimension{ texDimension }
    ,m_miplevels{ miplevels }
    ,m_vkImgFormat{ imgFormat }
    ,m_vkImgTiling{ imgTiling }
    ,m_vkImgUsageFlags{ imgUsageFlags }
    ,m_vkImgMemoryFlags{ memoryPropertyFlags }
    ,m_vkImgSampleCountFlags{ imgSampleCountFlags }
    ,m_vkImgAspect{ imgAspect }
{}

VulkanTexture::~VulkanTexture()
{
    m_pTextureManager->getDevice()->destroyImageView( m_vkImageView );
    m_pTextureManager->getDevice()->destroyImage( m_vkImage );
    m_pTextureManager->getDevice()->freeMemory( m_vkImgMemory );
}

void VulkanTexture::createImage()
{
    vk::ImageCreateInfo imgCreateInfo{};
    imgCreateInfo.imageType = vk::ImageType::e2D;
    imgCreateInfo.extent.width = m_texDimension.m_width;
    imgCreateInfo.extent.height = m_texDimension.m_height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.mipLevels = m_miplevels;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.format = m_vkImgFormat;
    imgCreateInfo.tiling = m_vkImgTiling;
    imgCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    imgCreateInfo.usage = m_vkImgUsageFlags;
    imgCreateInfo.samples = m_vkImgSampleCountFlags;
    imgCreateInfo.flags = {};

    vk::Device* pDevice = m_pTextureManager->getDevice();
    m_vkImage = pDevice->createImage( imgCreateInfo );

    vk::MemoryRequirements memRequirements = pDevice->getImageMemoryRequirements( m_vkImage );
    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanHelpers::findMemoryType(
        m_pTextureManager->getRenderer()->getPhysicalDevice(),
        memRequirements.memoryTypeBits, m_vkImgMemoryFlags
    );

    m_vkImgMemory = pDevice->allocateMemory( allocInfo );
    pDevice->bindImageMemory( m_vkImage, m_vkImgMemory, 0 );
}

void VulkanTexture::createImageView()
{
    vk::ImageViewCreateInfo imgViewCreateInfo{};
	imgViewCreateInfo.image = m_vkImage;
	imgViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imgViewCreateInfo.format = m_vkImgFormat;
    vk::ComponentMapping componentMap;
    componentMap.r = vk::ComponentSwizzle::eIdentity;
    componentMap.g = vk::ComponentSwizzle::eIdentity;
    componentMap.b = vk::ComponentSwizzle::eIdentity;
    componentMap.a = vk::ComponentSwizzle::eIdentity;
    imgViewCreateInfo.components = componentMap;
    vk::ImageSubresourceRange subResourceRange;
    subResourceRange.aspectMask = m_vkImgAspect;
    subResourceRange.baseMipLevel = 0;
    subResourceRange.levelCount = m_miplevels;
    subResourceRange.baseArrayLayer = 0;
    subResourceRange.layerCount = 1;
    imgViewCreateInfo.subresourceRange = subResourceRange;

    m_vkImageView = m_pTextureManager->getDevice()->createImageView( imgViewCreateInfo );
}

} // namespace vkrender
