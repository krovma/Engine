#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

class Texture
{
public:
	Texture();

	unsigned int	GetTextureID() const { return m_textureID; }
	IntVec2			GetTextureSize() const { return m_textureSize; }
	std::string		GetTextureName() const { return m_textureName; }

	void SetTextureID(unsigned int textureID);
	void SetTextureSize(const IntVec2& textureSize);
	void SetTextureSize(int textureSizeX, int textureSizeY);
	void SetTextureName(const char* name);

private:
	unsigned int	m_textureID = 0;	// 0 for unused
	IntVec2			m_textureSize;
	std::string		m_textureName;
};