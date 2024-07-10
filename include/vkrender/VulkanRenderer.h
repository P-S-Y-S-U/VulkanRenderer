#ifndef VKRENDER_VULKAN_RENDERER_H
#define VKRENDER_VULKAN_RENDERER_H

#include "vkrender/VulkanWindow.h"

#include "vkrender/VulkanRendererExports.hpp"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanRenderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void initVulkan( VulkanWindow* pVulkanWindow );
    void shutdown();

private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface( VulkanWindow* pVulkanWindow );
    void pickPhysicalDevice();
    void createLogicalDevice();

#ifdef NDEBUG
	static constexpr bool ENABLE_VALIDATION_LAYER = false;
#else
	static constexpr bool ENABLE_VALIDATION_LAYER = true;
#endif // NDEBUG

    vk::Instance m_vkInstance;
    vk::DebugUtilsMessengerEXT m_vkDebugUtilsMessenger;
    vk::SurfaceKHR m_vkSurface;

    vk::PhysicalDevice m_vkPhysicalDevice;
    vk::Device m_vkLogicalDevice;
    vk::Queue m_vkGraphicsQueue;
    vk::Queue m_vkPresentationQueue;
    vk::Queue m_vkTransferQueue;
    bool m_bHasExclusiveTransferQueue;

    std::vector<const char*> m_instanceExtensionContainer;
    std::vector<const char*> m_deviceExtensionContainer;

    VulkanWindow* m_pVulkanWindow;
};

} // namespace vkrender

#endif