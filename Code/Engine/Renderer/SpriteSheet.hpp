#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteDef.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class TextureView2D;
//////////////////////////////////////////////////////////////////////////
class SpriteSheet
{
public:
	SpriteSheet(const TextureView2D* texture, const IntVec2& layout);
	SpriteSheet(const SpriteSheet& copyFrom);
	~SpriteSheet();
	const TextureView2D* GetTexture() const { return m_texture; }
	const IntVec2 GetLayout() const { return m_layout; }
	const SpriteDef& GetSpriteDef(int spriteIndex) const { return m_spriteDefs[spriteIndex]; }

protected:
	const TextureView2D* m_texture = nullptr;
	const IntVec2 m_layout;
	std::vector<SpriteDef> m_spriteDefs;
};