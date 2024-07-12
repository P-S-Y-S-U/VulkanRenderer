#ifndef VKRENDER_VULKAN_SWAPCHAIN_H
#define VKRENDER_VULKAN_SWAPCHAIN_H

#include "vkrender/VulkanSwapChainStructs.hpp"
#include "vkrender/VulkanRendererExports.hpp"
#include "utilities/UtilityCommon.hpp"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

struct SwapchainCreateInfo
{
    vk::PhysicalDevice vkPhysicalDevice;
    vk::Device vkLogicalDevice;
    vk::SurfaceKHR vkSurface;
    vk::SampleCountFlags vkSampleCount;
    utils::Dimension framebufferSize;
};

class VULKANRENDERER_EXPORTS VulkanSwapchain
{
public:
    VulkanSwapchain( const SwapchainCreateInfo& swapchainCreateInfo );
    ~VulkanSwapchain();

    void createSwapchain();
    void recreateSwapchain( const utils::Dimension& framebufferDimension );
private:
    void destroySwapchain();
    void createSwapchainImageViews();
    void createColorResources();
    void createDepthResources();
    void createFramebuffers();

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat( const SwapChainSupportDetails& swapChainSupportDetails );
    vk::PresentModeKHR chooseSwapPresentMode( const SwapChainSupportDetails& swapChainSupportDetails );
    vk::Extent2D chooseSwapExtent( const SwapChainSupportDetails& swapChainSupportDetails, const utils::Dimension& framebufferSize );
    std::uint32_t chooseImageCount( const SwapChainSupportDetails& swapChainSupportDetails );

    vk::Format findDepthFormat();

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
    std::vector<vk::Framebuffer> m_vkSwapchainFramebuffers;

    vk::Image m_vkColorImage;
    vk::ImageView m_vkColorImageView;
    vk::DeviceMemory m_vkColorImageMemory;
    vk::Image m_vkDepthImage;
    vk::ImageView m_vkDepthImageView;
    vk::DeviceMemory m_vkDepthImageMemory;

    vk::SampleCountFlags m_vkSampleCount;
};

} // namespace vkrender

#endif 