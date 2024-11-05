#ifndef VKRENDER_VULKAN_RENDERER_H
#define VKRENDER_VULKAN_RENDERER_H

#include "vkrender/VulkanWindow.h"
#include "vkrender/VulkanSwapchain.h"
#include "vkrender/VulkanRendererExports.hpp"
#include "utilities/memory.hpp"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanRenderer
{
public:
    using CmdBufPtr = utils::Uptr<VulkanCmdBuffer>;

    VulkanRenderer();
    ~VulkanRenderer();

    void initVulkan( VulkanWindow* pVulkanWindow );
    void shutdown();

#ifdef NDEBUG
	static constexpr bool ENABLE_VALIDATION_LAYER = false;
#else
	static constexpr bool ENABLE_VALIDATION_LAYER = true;
#endif // NDEBUG

    void recreateSwapchain();
private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface( VulkanWindow* pVulkanWindow );
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();
    void createConfigCommandBuffer();

    vk::Instance m_vkInstance;
    vk::DebugUtilsMessengerEXT m_vkDebugUtilsMessenger;
    vk::SurfaceKHR m_vkSurface;

    vk::PhysicalDevice m_vkPhysicalDevice;
    vk::Device m_vkLogicalDevice;
    vk::Queue m_vkGraphicsQueue;
    vk::Queue m_vkPresentationQueue;
    vk::Queue m_vkTransferQueue;
    bool m_bHasExclusiveTransferQueue;
    vk::SampleCountFlagBits m_msaaSampleCount;

    std::vector<const char*> m_instanceExtensionContainer;
    std::vector<const char*> m_deviceExtensionContainer;

    vk::CommandPool m_vkTransferCommandPool;
    vk::CommandPool m_vkGraphicsCommandPool;

    CmdBufPtr m_pConfigCmdBuffer;
    
    VulkanWindow* m_pVulkanWindow;
    utils::Uptr<VulkanSwapchain> m_pVulkanSwapchain;
};

} // namespace vkrender

#endif