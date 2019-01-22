#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/SpriteDef.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class Texture;
//////////////////////////////////////////////////////////////////////////
class SpriteSheet
{
public:
	SpriteSheet(const Texture* texture, const IntVec2& layout);
	SpriteSheet(const SpriteSheet& copyFrom);
	~SpriteSheet();
	const Texture* GetTexture() const { return m_texture; }
	const IntVec2 GetLayout() const { return m_layout; }
	const SpriteDef& GetSpriteDef(int spriteIndex) const { return m_spriteDefs[spriteIndex]; }

protected:
	const Texture* m_texture = nullptr;
	const IntVec2 m_layout;
	std::vector<SpriteDef> m_spriteDefs;
};