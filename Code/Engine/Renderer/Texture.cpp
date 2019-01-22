#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"

#include <cstring>

////////////////////////////////
Texture::Texture()
{
}

////////////////////////////////
void Texture::SetTextureID(unsigned int textureID)
{
	m_textureID = textureID;
}

////////////////////////////////
void Texture::SetTextureSize(const IntVec2& textureSize)
{
	m_textureSize = textureSize;
}

////////////////////////////////
void Texture::SetTextureSize(int textureSizeX, int textureSizeY)
{
	m_textureSize.x = textureSizeX;
	m_textureSize.y = textureSizeY;
}

////////////////////////////////
void Texture::SetTextureName(const char* name)
{
	m_textureName = name;
}

#endif