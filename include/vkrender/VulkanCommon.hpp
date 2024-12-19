#ifndef VKRENDER_VULKAN_COMMON_HPP
#define VKRENDER_VULKAN_COMMON_HPP

#include <vector>

namespace vkrender
{

struct VulkanRenderTarget;
struct VulkanAttachmentBinding;

using VulkanRenderTargetArray = std::vector<VulkanRenderTarget>;
using VulkanAttachmentBindingArray = std::vector<VulkanAttachmentBinding>;

}

#endif