#ifndef VKRENDER_VULKAN_QUEUE_FAMILY_HPP
#define VKRENDER_VULKAN_QUEUE_FAMILY_HPP

#include <optional>

namespace vkrender
{
	struct QueueFamilyIndices
	{
		std::optional<std::uint32_t>	m_graphicsFamily;
		std::optional<std::uint32_t>	m_presentFamily;
		std::optional<std::uint32_t>	m_computeFamily;
		std::optional<std::uint32_t>	m_exclusiveTransferFamily;
	};

} // namespace vkrender

#endif