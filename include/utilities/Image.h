#ifndef UTILS_IMAGE_H
#define UITLS_IMAGE_H

#include "vkrender/VulkanRendererExports.hpp"
#include "utilities/UtilityCommon.hpp"

#include <filesystem>

namespace utils
{
    class VULKANRENDERER_EXPORTS Image
    {
    public:
        using ImgBufPtr = unsigned char*;

        Image();
        Image( const Dimension& dimension, const uint32_t& channels, ImgBufPtr buffer );
        ~Image();

        void loadBuffer( const std::filesystem::path& path );
        void clearBuffer();

        std::size_t sizeInBytes() const;

        uint32_t miplevels() const { return m_mipLevels; }
        uint32_t colorChannels() const { return m_colorChannels; }
        Dimension dimension() const { return m_imgDimension; }
        const ImgBufPtr buffer() const { return m_buffer; }
    private:
        Dimension m_imgDimension;
        ImgBufPtr m_buffer;
        std::uint32_t m_colorChannels;
        std::uint32_t m_mipLevels;

        void calculateMiplevels();
    };
} // namespace utils

#endif