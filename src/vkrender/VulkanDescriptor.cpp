#include "vkrender/VulkanDescriptor.h"

namespace vkrender
{

VulkanDescriptor::VulkanDescriptor( 
    vk::Device* pLogicalDevice, const std::uint32_t& numOfSetsPerPool
)
    :m_pLogicalDevice{ pLogicalDevice }
    ,m_numOfSetsPerPool{ numOfSetsPerPool }
{}

VulkanDescriptor::~VulkanDescriptor()
{
    destroyDescriptorSet();
}

void VulkanDescriptor::allocateDescriptorSets( const DescriptorBindingArray& bindings )
{
    createDescriptorPool( bindings );
    createDescriptorPool( bindings );
    createDescriptorSets();
    m_vkWriteDescriptorSets.resize( bindings.size() );
}
    
vk::WriteDescriptorSet* VulkanDescriptor::getWriteDescriptor( const std::uint32_t& bindingIndex )
{
    return &m_vkWriteDescriptorSets[bindingIndex];
}

void VulkanDescriptor::updateDescriptorSets()
{
    for( std::uint32_t i = 0; i < m_numOfSetsPerPool; i++ )
    {
        m_pLogicalDevice->updateDescriptorSets(
            m_vkWriteDescriptorSets.size(), m_vkWriteDescriptorSets.data(),
            0, nullptr
        );
    }
}

void VulkanDescriptor::createDescriptorSetLayout( const DescriptorBindingArray& bindings )
{
    const std::uint32_t numOfBindings = static_cast<std::uint32_t>( bindings.size() );

    std::vector<vk::DescriptorSetLayoutBinding> vkDescSetLayoutBindings{ numOfBindings };

    for( std::uint32_t i = 0; i < numOfBindings; i++ )
    {
        vk::DescriptorSetLayoutBinding& descSetLayoutBinding = vkDescSetLayoutBindings[i];
        descSetLayoutBinding.binding = i;
        descSetLayoutBinding.descriptorType = bindings[i].m_bindingType;
        descSetLayoutBinding.descriptorCount = 1;
        descSetLayoutBinding.stageFlags = bindings[i].m_stageFlags;
        descSetLayoutBinding.pImmutableSamplers = nullptr;
    }

    vk::DescriptorSetLayoutCreateInfo descLayoutInfo{};
    descLayoutInfo.bindingCount = numOfBindings;
    descLayoutInfo.pBindings = vkDescSetLayoutBindings.data();

    m_vkDescriptorSetLayout = m_pLogicalDevice->createDescriptorSetLayout( descLayoutInfo );
}

void VulkanDescriptor::createDescriptorPool( const DescriptorBindingArray& bindings )
{
    const std::uint32_t numOfBindings = static_cast<std::uint32_t>( bindings.size() );

    std::vector<vk::DescriptorPoolSize> vkDescPoolSizes{ numOfBindings };
    
    for( std::uint32_t i = 0u; i < numOfBindings; i++ )
    {
        vk::DescriptorPoolSize& descPoolSize = vkDescPoolSizes[i];
        descPoolSize.type = bindings[i].m_bindingType;
        descPoolSize.descriptorCount = m_numOfSetsPerPool;        
    }

    vk::DescriptorPoolCreateInfo descPoolInfo{};
    descPoolInfo.poolSizeCount = numOfBindings;
    descPoolInfo.pPoolSizes = vkDescPoolSizes.data();
    descPoolInfo.maxSets = m_numOfSetsPerPool;

    m_vkDescriptorPool = m_pLogicalDevice->createDescriptorPool( descPoolInfo );
}

void VulkanDescriptor::createDescriptorSets()
{
    std::vector<vk::DescriptorSetLayout> descLayouts{ m_numOfSetsPerPool, m_vkDescriptorSetLayout };

    vk::DescriptorSetAllocateInfo descSetAllocInfo{};
    descSetAllocInfo.descriptorPool = m_vkDescriptorPool;
    descSetAllocInfo.descriptorSetCount = m_numOfSetsPerPool;
    descSetAllocInfo.pSetLayouts = descLayouts.data();

    m_vkDescriptorSets = m_pLogicalDevice->allocateDescriptorSets( descSetAllocInfo );
}

void VulkanDescriptor::destroyDescriptorSet()
{
    m_pLogicalDevice->destroyDescriptorPool( m_vkDescriptorPool );
    m_pLogicalDevice->destroyDescriptorSetLayout( m_vkDescriptorSetLayout );
}

} // namespace vkrender 