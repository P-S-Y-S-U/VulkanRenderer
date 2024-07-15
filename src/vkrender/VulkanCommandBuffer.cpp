#include "vkrender/VulkanCommandBuffer.h"

namespace vkrender
{

vk::CommandBuffer* VulkanImmediateCmdBuffer::beginCmdBuffer()
{
    m_vkCmdBuffer.reset( {} );

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    m_vkCmdBuffer.begin( beginInfo );

    return &m_vkCmdBuffer;
}

void VulkanImmediateCmdBuffer::endCmdBuffer()
{
    m_vkCmdBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_vkCmdBuffer;

    vk::Result opResult = m_pSubmitionQueue->submit( 1, &submitInfo, {} );
    m_pLogicalDevice->waitIdle();
}

vk::CommandBuffer* VulkanTemporaryCmdBuffer::beginCmdBuffer()
{
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    m_vkCmdBuffer.begin( beginInfo );

    return &m_vkCmdBuffer;
}

void VulkanTemporaryCmdBuffer::endCmdBuffer()
{
    m_vkCmdBuffer.end();

    vk::SubmitInfo submitInfo{};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_vkCmdBuffer;

	vk::Result opResult = m_pSubmitionQueue->submit( 1, &submitInfo, nullptr );
	m_pSubmitionQueue->waitIdle();

	m_pLogicalDevice->freeCommandBuffers( *m_pCommandPool, 1, &m_vkCmdBuffer );
}

} // namespace vkrender