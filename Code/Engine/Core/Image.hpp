#pragma once
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <vector>
#include <map>

class Image
{
public:
	static Image* AcquireImage(const char* path);
	
	explicit Image(const char* path);
	explicit Image(int w, int h, const char* name);
	//#Todo explicit Image(const Rgba* data);
	const Rgba& GetTexel(int u, int v) const;
	const Rgba& GetTexel(int texelIndex) const;
	const IntVec2& GetImageSize() const;
	const std::string& GetPath() const;
	const unsigned char* GetRawImageData() const;
	void SetTexelColor(const IntVec2& uv, const Rgba& color);
	void SetTexelColor(int u, int v, const Rgba& color);

private:
	std::string m_path;
	IntVec2 m_imageSize;
	std::vector<Rgba> m_data;
	unsigned char* m_rawData;

	static std::map<std::string, Image*> s_LoadedImages;
};