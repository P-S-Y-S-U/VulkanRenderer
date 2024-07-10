#ifndef VKRENDER_VULKANRENDERER_EXPORTS_HPP
#define VKRENDER_VULKANRENDERER_EXPORTS_HPP

#ifdef WIN32
    #ifdef vulkanrenderer_EXPORTS
        #define VULKANRENDERER_EXPORTS __declspec(dllexport)
    #else
        #define VULKANRENDERER_EXPORTS __declspec(dllimport)
    #endif // #ifdef vulkanrenderer_EXPORTS
#else
    #define VULKANRENDERER_EXPORTS
#endif // #ifdef WIN32 

#endif