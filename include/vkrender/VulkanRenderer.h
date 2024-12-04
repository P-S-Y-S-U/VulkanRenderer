#ifndef VKRENDER_VULKAN_RENDERER_H
#define VKRENDER_VULKAN_RENDERER_H

#include "vkrender/VulkanWindow.h"
#include "vkrender/VulkanSwapchain.h"
#include "vkrender/VulkanRenderPass.h"
#include "vkrender/VulkanCommon.hpp"
#include "vkrender/VulkanRendererExports.hpp"
#include "utilities/memory.hpp"

#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VulkanTextureManager;

class VULKANRENDERER_EXPORTS VulkanRenderer
{
public:
    using CmdBufPtr = utils::Uptr<VulkanCmdBuffer>;

    VulkanRenderer();
    ~VulkanRenderer();

    void initVulkan( VulkanWindow* pVulkanWindow );
    void shutdown();
    
    vk::PhysicalDevice getPhysicalDevice() const { return m_vkPhysicalDevice; }
#ifdef NDEBUG
	static constexpr bool ENABLE_VALIDATION_LAYER = false;
#else
	static constexpr bool ENABLE_VALIDATION_LAYER = true;
#endif // NDEBUG

    void destroySwapchain();
    void recreateSwapchain();

    void createBuffer(
        const vk::DeviceSize& bufferSizeInBytes,
        const vk::BufferUsageFlags& bufferUsage, const vk::SharingMode& bufferSharing,
        const vk::MemoryPropertyFlags& memProps,
        vk::Buffer& buffer, vk::DeviceMemory& bufferMemory
    );

    vk::Sampler* createTexSampler(
        const vk::Filter& minFilter, const vk::Filter& magFilter,
        const vk::SamplerAddressMode& uAddrMode, const vk::SamplerAddressMode& vAddrMode, const vk::SamplerAddressMode& wAddrMode,
        const bool& bEnableAnisotropy,
        const vk::SamplerMipmapMode& mipmapMode,
        const float& minLod, const float& maxLod, const float& mipLodBias = 0.0f,
        const vk::BorderColor& borderColor = vk::BorderColor::eIntOpaqueBlack,
        const bool& bnormalizedCoords = true,
        const bool& bCmpEnable = false, const vk::CompareOp& cmpOp = vk::CompareOp::eAlways
    );

    vk::Framebuffer createFramebuffer(
        const VulkanRenderPass& renderPassToBind,
        const VulkanRenderTargetArray& renderTargets,
        const utils::Dimension& framebufferSize,
        const std::uint32_t& layers
    );
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

    std::vector<vk::Sampler> m_samplers;

    friend class VulkanTextureManager;
};

} // namespace vkrender

#endif