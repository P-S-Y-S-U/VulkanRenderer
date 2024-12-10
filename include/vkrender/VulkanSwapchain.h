#ifndef VKRENDER_VULKAN_SWAPCHAIN_H
#define VKRENDER_VULKAN_SWAPCHAIN_H

#include "vkrender/VulkanSwapChainStructs.hpp"
#include "vkrender/VulkanCommon.hpp"
#include "vkrender/VulkanRenderPass.h"
#include "vkrender/VulkanRendererExports.hpp"
#include "vkrender/VulkanCommandBuffer.h"
#include "utilities/UtilityCommon.hpp"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

struct SwapchainCreateInfo
{
    vk::PhysicalDevice vkPhysicalDevice;
    vk::Device vkLogicalDevice;
    vk::SurfaceKHR vkSurface;
    vk::SampleCountFlagBits vkSampleCount;
};

class VULKANRENDERER_EXPORTS VulkanSwapchain
{
public:
    VulkanSwapchain( const SwapchainCreateInfo& swapchainCreateInfo );
    ~VulkanSwapchain();

    void createSwapchain( const utils::Dimension& framebufferDimension );
    void destroySwapchain();
    void createSwapchainFramebuffers( const VulkanRenderPass& passToBind, const VulkanRenderTargetArray& prependRTs );
    void prepareSwapchainAttachment( VulkanRenderTarget& swapchainRT );
    void recreateSwapchain( const utils::Dimension& framebufferDimension, const VulkanRenderPass& passToBind, const VulkanRenderTargetArray& prependRTs );
private:
    void createSwapchainImageViews();

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat( const SwapChainSupportDetails& swapChainSupportDetails );
    vk::PresentModeKHR chooseSwapPresentMode( const SwapChainSupportDetails& swapChainSupportDetails );
    vk::Extent2D chooseSwapExtent( const SwapChainSupportDetails& swapChainSupportDetails, const utils::Dimension& framebufferSize );
    std::uint32_t chooseImageCount( const SwapChainSupportDetails& swapChainSupportDetails );

    vk::Format findDepthFormat();

    vk::Framebuffer VulkanSwapchain::createFramebuffer(
        const VulkanRenderPass& renderPassToBind,
        const VulkanRenderTargetArray& renderTargets,
        const utils::Dimension& framebufferSize,
        const std::uint32_t& layers
    );

    vk::PhysicalDevice m_vkPhysicalDevice;
    vk::Device m_vkLogicalDevice;
    vk::SurfaceKHR m_vkSurface;

    utils::Dimension m_framebufferSize;

    vk::SwapchainKHR m_vkSwapchain;
    vk::SharingMode m_vkSwapchainImageSharingMode;
    vk::Format m_vkSwapchainImageFormat;
    vk::Format m_vkDepthImageFormat;
    vk::Extent2D m_vkSwapchainExtent;
    std::vector<vk::Image> m_vkSwapchainImages;
    std::vector<vk::ImageView> m_vkSwapchainImageViews;
    std::vector<vk::Framebuffer> m_swapchainFramebuffers;
    vk::SampleCountFlagBits m_vkSampleCount;
};

} // namespace vkrender

#endif 