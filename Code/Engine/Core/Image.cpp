#include "Engine/Core/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"
//////////////////////////////////////////////////////////////////////////
STATIC std::map<std::string, Image*> Image::s_LoadedImages;
////////////////////////////////
Image* Image::AcquireImage(const char* path)
{
	auto findedImage = s_LoadedImages.find(std::string(path));
	if (findedImage != s_LoadedImages.end()) {
		return findedImage->second;
	} else {
		Image* createdImage = new Image(path);
		s_LoadedImages[std::string(path)] = createdImage;
		return createdImage;
	}
}

////////////////////////////////
Image::Image(const char* path)
{
	stbi_set_flip_vertically_on_load(1);
	int numComponents;
	unsigned char* imageData = 
		stbi_load(path, &m_imageSize.x, &m_imageSize.y, &numComponents, 0);
	GUARANTEE_OR_DIE(imageData, Stringf("Image %s can't be loaded", path));
	int numTexels = m_imageSize.x * m_imageSize.y;
	m_data.reserve(numTexels);
	for (int texelIndex = 0; texelIndex < numTexels; ++texelIndex) {
		int offset = texelIndex * numComponents;
		unsigned int r = imageData[offset];
		unsigned int g = imageData[offset + 1];
		unsigned int b = imageData[offset + 2];
		unsigned int a = 255u;
		if (numComponents == 4) {
			a = imageData[offset + 3];
		}
		m_data.push_back(Rgba(r, g, b, a));
	}
	stbi_image_free(imageData);
}

////////////////////////////////
const Rgba& Image::GetTexel(int texelIndex) const
{
	return m_data[texelIndex];
}

////////////////////////////////
const Rgba& Image::GetTexel(int u, int v) const
{
	return GetTexel(v * m_imageSize.x + u);
}

////////////////////////////////
const IntVec2& Image::GetImageSize() const
{
	return m_imageSize;
}

////////////////////////////////
const std::string& Image::GetPath() const
{
	return m_path;
}

////////////////////////////////
void Image::SetTexelColor(const IntVec2& uv, const Rgba& color)
{
	SetTexelColor(uv.x, uv.y, color);
}

////////////////////////////////
void Image::SetTexelColor(int u, int v, const Rgba& color)
{
	m_data[v * m_imageSize.x + u] = color;
}
