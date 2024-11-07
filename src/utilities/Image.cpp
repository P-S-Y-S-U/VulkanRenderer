#include "utilities/Image.h"
#include "utilities/VulkanLogger.h"

#include <stb/stb_image.h>
#include <spdlog/spdlog.h>

namespace utils
{

Image::Image()
{}

Image::Image( const Dimension& dimension, const uint32_t& channels, ImgBufPtr buffer )
    :m_imgDimension{ dimension }
    ,m_colorChannels{ channels }
    ,m_buffer{ buffer }
{
    calculateMiplevels();
}

Image::~Image()
{
    clearBuffer();
}

void Image::loadBuffer( const std::filesystem::path& path )
{
    if( !std::filesystem::exists(path) )
    {
        LOG_INFO( fmt::format("{} Path does not exist", path.string()) );
		return;
    }

    int width;
    int height;
    int channel;

    m_buffer = stbi_load(
		path.string().data(),
		&width, &height, &channel,
		STBI_rgb_alpha
	);

    m_imgDimension.m_width = width;
    m_imgDimension.m_height = height;
    m_colorChannels = channel;

	if(!m_buffer)
	{
		std::string errorMsg = fmt::format("Failed to load {} image", path.string());
		LOG_ERROR(errorMsg);
		throw  std::runtime_error(errorMsg);
	}

    calculateMiplevels();
}

void Image::clearBuffer()
{
    if( m_buffer )
    {
        stbi_image_free(m_buffer);
    }
}

std::size_t Image::sizeInBytes() const
{
    return m_imgDimension.m_width * m_imgDimension.m_height * 4;
}

void Image::calculateMiplevels()
{
    m_mipLevels = static_cast<std::uint32_t>( std::floor( std::log2( std::max( m_imgDimension.m_width, m_imgDimension.m_height ) ) ) ) + 1;
}

} // namespace utils
