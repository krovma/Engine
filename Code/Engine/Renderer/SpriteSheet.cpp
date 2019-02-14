#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/TextureView2D.hpp"

////////////////////////////////
SpriteSheet::SpriteSheet(const TextureView2D* texture, const IntVec2& layout)
	: m_texture(texture)
	, m_layout(layout)
{
	float uStride = 1.f / (float)layout.x;
	float vStride = 1.f / (float)layout.y;
	int numSprites = layout.x * layout.y;

	for (int spriteIndex = 0; spriteIndex < numSprites; ++spriteIndex) {
		IntVec2 spriteGridCoord(spriteIndex % layout.x, spriteIndex / layout.x);
		float uBottomLeft = uStride * (float)spriteGridCoord.x;
		float uTopRight = uBottomLeft + uStride;
		float vBottomLeft = (vStride * (float)(spriteGridCoord.y + 1));
		float vTopRight = vBottomLeft - vStride;
		m_spriteDefs.push_back(SpriteDef(spriteIndex, Vec2(uBottomLeft, vBottomLeft), Vec2(uTopRight, vTopRight)));
	}
}

////////////////////////////////
SpriteSheet::SpriteSheet(const SpriteSheet& copyFrom)
	:SpriteSheet(copyFrom.m_texture, copyFrom.m_layout)
{
}

////////////////////////////////
SpriteSheet::~SpriteSheet()
{

}

#endif