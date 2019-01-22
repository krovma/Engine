#pragma once
#include "Engine/renderer/SpriteSheet.hpp"

enum SpriteAnimationMode
{
	SPRITE_ANIMATION_ONCE,
	SPRITE_ANIMATION_LOOP,
	SPRITE_ANIMATION_PINGPONG,

	NUM_SPRITE_ANIMATION_TYPES
};

class SpriteAnimationDef
{
public:
	SpriteAnimationDef(const SpriteSheet& spriteSheet
						, int startFrameIndex
						, int endFrameIndex
						, float durationSeconds
						, SpriteAnimationMode animationMode = SpriteAnimationMode::SPRITE_ANIMATION_LOOP);

	const SpriteDef& GetFrameAtTime(float seconds) const;
	float GetDurationOfFrame(int frameNumber) const;
	void SetDuration(float duration) { m_duration = duration; }
	float GetDuration() const { return m_duration; }
	const Texture* GetAnimSpriteTexture() const { return m_spriteSheet.GetTexture(); }
private:
	const SpriteSheet/*&*/ m_spriteSheet;
	int m_startIndex = -1;
	int m_endIndex = -1;
	int m_frameTotal = 0;
	float m_duration = 1.f;
	SpriteAnimationMode m_animationMode = SPRITE_ANIMATION_LOOP;
};