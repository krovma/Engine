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
	//stbi_set_flip_vertically_on_load(1);
	int numComponents;
	m_rawData =	stbi_load(path, &m_imageSize.x, &m_imageSize.y, &numComponents, 0);
	GUARANTEE_OR_DIE(m_rawData, Stringf("Image %s can't be loaded", path));
	int numTexels = m_imageSize.x * m_imageSize.y;
	m_data.reserve(numTexels);
	for (int texelIndex = 0; texelIndex < numTexels; ++texelIndex) {
		int offset = texelIndex * numComponents;
		unsigned int r = m_rawData[offset];
		unsigned int g = m_rawData[offset + 1];
		unsigned int b = m_rawData[offset + 2];
		unsigned int a = 255u;
		if (numComponents == 4) {
			a = m_rawData[offset + 3];
		}
		m_data.push_back(Rgba(r, g, b, a));
	}
	//stbi_image_free(imageData);
}

////////////////////////////////
Image::Image(int w, int h, const char* name)
	:m_path(name), m_imageSize(w, h)
{
	m_data.resize(w * h);
	m_rawData = new unsigned char[w * h * 4];
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
const unsigned char* Image::GetRawImageData() const
{
	return m_rawData;
}

////////////////////////////////
void Image::SetTexelColor(const IntVec2& uv, const Rgba& color)
{
	SetTexelColor(uv.x, uv.y, color);
}

////////////////////////////////
void Image::SetTexelColor(int u, int v, const Rgba& color)
{
	int texelIndex = v * m_imageSize.x + u;
	m_data[texelIndex] = color;
	unsigned char r = (unsigned char)(color.r * 255u);
	unsigned char g = (unsigned char)(color.g * 255u);
	unsigned char b = (unsigned char)(color.b * 255u);
	unsigned char a = (unsigned char)(color.a * 255u);
	int offset = texelIndex * 4;
	m_rawData[offset] = r;
	m_rawData[offset + 1] = g;
	m_rawData[offset + 2] = b;
	m_rawData[offset + 3] = a;
}
