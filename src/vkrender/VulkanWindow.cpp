#include "vkrender/VulkanWindow.h"

namespace vkrender
{
	VulkanWindow::VulkanWindow( const std::uint32_t& width, const std::uint32_t& height )
		:m_pWindow{ nullptr }
		,m_windowDimension{ width, height }
		,m_bQuit{ false }
		,m_bFrameBufferResized{ false }
	{}

	VulkanWindow::VulkanWindow()
		:m_pWindow{ nullptr }
		,m_windowDimension{ 800, 600 }
		,m_bQuit{ false }
		,m_bFrameBufferResized{ false }
	{
	}

	VulkanWindow::~VulkanWindow()
	{
		destroy();
	}

	void VulkanWindow::init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_pWindow = glfwCreateWindow(m_windowDimension.m_width, m_windowDimension.m_height, "Vulkan", nullptr, nullptr);
		glfwSetWindowUserPointer( m_pWindow, this );

		glfwSetFramebufferSizeCallback( m_pWindow, VulkanWindow::framebufferResizeCallback );
	}

/*
	utils::Uptr<VulkanSurface> VulkanWindow::createSurface( VulkanInstance* pInstance )
	{
		utils::Uptr<vk::SurfaceKHR> upSurfaceHandle = std::make_unique<vk::SurfaceKHR>();
		if( glfwCreateWindowSurface( 
			pInstance->m_instance, 
			m_pWindow, 
			nullptr, 
			reinterpret_cast<vk::SurfaceKHR::NativeType*>( upSurfaceHandle.get() )
			)
		)
		{
			throw std::runtime_error("Failed to create VulkanWindow Surface!");
		}
		VulkanSurface* pVulkanSurface = new VulkanSurface( pInstance, std::move(upSurfaceHandle));
		utils::Uptr<VulkanSurface> upSurfaceWrapper{ pVulkanSurface };

		return std::move(upSurfaceWrapper);
	}
*/

	void VulkanWindow::processEvents()
	{
		m_bQuit = glfwWindowShouldClose(m_pWindow);

		if( !m_bQuit )
		{
			glfwPollEvents();
		}
	}

	void VulkanWindow::destroy()
	{
		if( m_pWindow )
		{
			glfwDestroyWindow(m_pWindow);
		}
		glfwTerminate();
	}

	bool VulkanWindow::quit() const 
	{
		return m_bQuit;
	}

	VulkanWindow::PlatformWindowHandle VulkanWindow::getHandle() const 
	{
#ifdef _WIN32	
		return glfwGetWin32Window( m_pWindow );
#else
		return glfwGetWaylandWindow( m_pWindow );
#endif
	}

#ifndef _WIN32
	VulkanWindow::PlatformDisplayHandle VulkanWindow::getDisplayHandle() const
	{
		return glfwGetWaylandDisplay();
	}
#endif

	utils::Dimension VulkanWindow::getDimensions() const
	{
		return m_windowDimension;
	}

	utils::Dimension VulkanWindow::getFrameBufferSize() const
	{
		std::int32_t frameBufferWidth = 0;
		std::int32_t frameBufferHeight = 0;

		glfwGetFramebufferSize( m_pWindow, &frameBufferWidth, &frameBufferHeight );
		while (frameBufferWidth == 0 || frameBufferHeight == 0)
		{
			glfwGetFramebufferSize(m_pWindow, &frameBufferWidth, &frameBufferHeight);
			glfwWaitEvents();
		}

		return utils::Dimension{ static_cast<std::uint32_t>( frameBufferWidth ), static_cast<std::uint32_t>( frameBufferHeight ) };
	}

	bool VulkanWindow::isFrameBufferResized()
	{
		if( m_bFrameBufferResized )
		{
			m_bFrameBufferResized = false;
			return true;
		}
		
		return false;
	}

	std::vector<const char*> VulkanWindow::populateAvailableExtensions()
	{
		glfwInit();
		
		std::uint32_t extensionCount = 0;
		const char** extensionNames;
		extensionNames = glfwGetRequiredInstanceExtensions( &extensionCount );

		std::vector<const char*> extensionContainer( extensionNames, extensionNames + extensionCount );

		return extensionContainer;
	}

	void VulkanWindow::framebufferResizeCallback( GLFWwindow* pNativeWindow, int width, int height )
	{
		VulkanWindow* pWindow = reinterpret_cast<VulkanWindow*>( glfwGetWindowUserPointer(pNativeWindow) );
		pWindow->m_bFrameBufferResized = true;
	}

} // namespace vkrender