#include "vkrender/VulkanWindow.h"
#include "vkrender/VulkanRenderer.h"

int main()
{
    using namespace vkrender;

    VulkanWindow vkWindow{ 800, 600 };

    vkWindow.init();

    VulkanRenderer vkRenderer;

    vkRenderer.initVulkan( &vkWindow );
    
    return EXIT_SUCCESS;
}