# project files src files list #
set(PROJECT_SRC_FILES       vkrender/VulkanWindow.cpp
                            vkrender/VulkanDebugMessenger.cpp
                            utilities/Image.cpp
                            utilities/VulkanLogger_VulkanValidationLayerLogger.cpp
                            utilities/VulkanLogger_VulkanRendererApiLogger.cpp
                            vkrender/VulkanRenderer.cpp
                            vkrender/VulkanRenderer_instance.cpp
                            vkrender/VulkanRenderer_device.cpp
                            vkrender/VulkanSwapchain.cpp
                            vkrender/VulkanCommandBuffer.cpp
                            vkrender/VulkanRenderPass.cpp
                            vkrender/VulkanDescriptor.cpp
                            vkrender/VulkanGpuProgram.cpp
                            vkrender/VulkanRenderTarget.cpp
                            vkrender/VulkanGfxPipeline.cpp
                            vkrender/VulkanHelpers.cpp
                            vkrender/VulkanTexture.cpp
                            vkrender/VulkanTextureManager.cpp                                                                                                                                                                                                                   
)

# library & executable config #
add_library(vulkanrenderer SHARED ${PROJECT_SRC_FILES})
target_include_directories(vulkanrenderer PUBLIC        $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
                                                        $<BUILD_INTERFACE:${DEPENDENCIES_INCLUDE_DIR}>
                                                        $<BUILD_INTERFACE:${Vulkan_INCLUDE_DIR}>
                                                        $<INSTALL_INTERFACE:include>
                                                        )
target_compile_definitions(vulkanrenderer PUBLIC ${PROJECT_COMPILER_DEFINITIONS})
target_link_libraries(vulkanrenderer PUBLIC glm::glm glfw spdlog::spdlog tinyobjloader ${Vulkan_LIBRARY} ) 

get_target_property(VULKANRENDERPROPERTY vulkanrenderer INCLUDE_DIRECTORIES)
message(${VULKANRENDERPROPERTY})
# cmake install config #
install(TARGETS vulkanrenderer  EXPORT ${_EXPORT_PREFIX}
                                RUNTIME DESTINATION ${PROJECT_BIN}
                                LIBRARY DESTINATION ${PROJECT_LIB}
                                ARCHIVE DESTINATION ${PROJECT_ARCHIVE}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include DESTINATION ${PROJECT_INSTALL}/include)