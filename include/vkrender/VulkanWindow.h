#ifndef VKRENDER_WINDOW_H
#define VKRENDER_WINDOW_H

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_KHR_WAYLAND_SURFACE
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#else
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#include <GLFW/glfw3native.h>

#include <cstdint>
#include <utility>
#include <vector>

#include "vkrender/VulkanRendererExports.hpp"
#include "utilities/UtilityCommon.hpp"

namespace vkrender
{
	class VULKANRENDERER_EXPORTS VulkanWindow
	{
	public:
#ifdef _WIN32
		using PlatformWindowHandle = HWND;
#else
		using PlatformWindowHandle = wl_surface*;
		using PlatformDisplayHandle = wl_display*;
#endif 
		explicit VulkanWindow( const std::uint32_t& width, const std::uint32_t& height );
		explicit VulkanWindow();
		VulkanWindow(const VulkanWindow&) = delete;
		VulkanWindow(VulkanWindow&&) noexcept = delete;
		~VulkanWindow();
		
		VulkanWindow& operator=(const VulkanWindow&) noexcept = delete;
		VulkanWindow& operator=(VulkanWindow&&) noexcept = delete;

		void init();
		// utils::Uptr<VulkanSurface> createSurface( VulkanInstance* pInstance );
		void processEvents();
		void destroy();
		bool quit() const;

		PlatformWindowHandle getHandle() const;
#ifndef _WIN32
		PlatformDisplayHandle getDisplayHandle() const;
#endif

		utils::Dimension getDimensions() const ;
		utils::Dimension getFrameBufferSize() const ;
		
		// resets to false after returning
		bool isFrameBufferResized();

		static std::vector<const char*> populateAvailableExtensions();

		static void framebufferResizeCallback(GLFWwindow* pWindow, int width, int height);
	private:
		GLFWwindow*		m_pWindow;

		utils::Dimension m_windowDimension;		

		bool m_bQuit;
		bool m_bFrameBufferResized;
	};

} // namespace vkrender

#endif

