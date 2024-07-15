#ifndef VKRENDER_VULKAN_COMMAND_BUFFER_H
#define VKRENDER_VULKAN_COMMAND_BUFFER_H


#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VulkanCmdBuffer
{
public:
    VulkanCmdBuffer(
        vk::Device* pLogicalDevice,    
        vk::Queue* pQueueToUse,
        vk::CommandPool* pCommandPool
    )
        :m_pLogicalDevice{ pLogicalDevice }
        ,m_pSubmitionQueue{ pQueueToUse }
        ,m_pCommandPool{ pCommandPool }
    {}

    virtual ~VulkanCmdBuffer() = default;

    vk::CommandBuffer* handle() { return &m_vkCmdBuffer; }

    virtual void allocate()
    {
        vk::CommandBufferAllocateInfo allocInfo{};
	    allocInfo.commandPool = *m_pCommandPool;
	    allocInfo.level = vk::CommandBufferLevel::ePrimary;
	    allocInfo.commandBufferCount = 1;

	    m_vkCmdBuffer = m_pLogicalDevice->allocateCommandBuffers(allocInfo)[0];
    }

    virtual vk::CommandBuffer* beginCmdBuffer() = 0;
    virtual void endCmdBuffer() = 0;

protected:
    vk::Device* m_pLogicalDevice;
    vk::Queue* m_pSubmitionQueue;
    vk::CommandPool* m_pCommandPool;

    vk::CommandBuffer m_vkCmdBuffer;
};

class VulkanImmediateCmdBuffer : public VulkanCmdBuffer
{
public:
    VulkanImmediateCmdBuffer(
        vk::Device* pLogicalDevice,
        vk::Queue* pQueueToUse,
        vk::CommandPool* pCommandPool
    )
        :VulkanCmdBuffer{ pLogicalDevice, pQueueToUse, pCommandPool }
    {}

    ~VulkanImmediateCmdBuffer() = default;

    vk::CommandBuffer* beginCmdBuffer() override;
    void endCmdBuffer() override;
};

class VulkanTemporaryCmdBuffer : public VulkanCmdBuffer
{
public:
    VulkanTemporaryCmdBuffer(
        vk::Device* pLogicalDevice,
        vk::Queue* pQueueToUse,
        vk::CommandPool* pCommandPool
    )
        :VulkanCmdBuffer{ pLogicalDevice, pQueueToUse, pCommandPool }
    {}

    ~VulkanTemporaryCmdBuffer() = default;

    vk::CommandBuffer* beginCmdBuffer() override;
    void endCmdBuffer() override;
};

}

#endif 