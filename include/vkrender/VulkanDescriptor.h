#ifndef VKRENDER_VULKAN_DESCRIPTOR_H
#define VKRENDER_VULKAN_DESCRIPTOR_H

#include "vkrender/VulkanRendererExports.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace vkrender
{

class VULKANRENDERER_EXPORTS VulkanDescriptor
{
public:
    struct DescriptorBinding
    {
        explicit DescriptorBinding(
            const vk::DescriptorType& bindingType, 
            const vk::ShaderStageFlagBits& stageFlags
        )
            :m_bindingType{ bindingType }
            ,m_stageFlags{ stageFlags }
        {}

        const vk::DescriptorType m_bindingType;
        const vk::ShaderStageFlagBits m_stageFlags;
    };

    using DescriptorBindingArray = std::vector<DescriptorBinding>;

    explicit VulkanDescriptor( vk::Device* pLogicalDevice, const std::uint32_t& numOfSetsPerPool = 1 );
    ~VulkanDescriptor();

    void allocateDescriptorSets( const DescriptorBindingArray& bindings );
    
    vk::WriteDescriptorSet* getWriteDescriptor( const std::uint32_t& bindingIndex );
    void updateDescriptorSets();
private:
    vk::Device* m_pLogicalDevice;
    std::uint32_t m_numOfSetsPerPool;

    vk::DescriptorSetLayout m_vkDescriptorSetLayout;
    vk::DescriptorPool m_vkDescriptorPool;

    std::vector<vk::DescriptorSet> m_vkDescriptorSets;
    std::vector<vk::WriteDescriptorSet> m_vkWriteDescriptorSets;

    void createDescriptorSetLayout( const DescriptorBindingArray& bindings );
    void createDescriptorPool( const DescriptorBindingArray& bindings );
    void createDescriptorSets();

    void destroyDescriptorSet();
};

} // namespace vkrender

#endif 