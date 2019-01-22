#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Renderer/SpriteAnimationDef.hpp"
#include "Engine/Core/EngineCommon.hpp"


////////////////////////////////
SpriteAnimationDef::SpriteAnimationDef(const SpriteSheet& spriteSheet
										, int startFrameIndex
										, int endFrameIndex
										, float durationSeconds
										, SpriteAnimationMode animationMode /*= SpriteAnimationMode::SPRITE_ANIMATION_LOOP*/)
	:m_spriteSheet(spriteSheet)
	,m_startIndex(startFrameIndex)
	,m_endIndex(endFrameIndex)
	,m_duration(durationSeconds)
	,m_animationMode(animationMode)
{
	if (m_animationMode != SPRITE_ANIMATION_PINGPONG) {
		m_frameTotal = m_endIndex - m_startIndex + 1;
	} else {
		m_frameTotal = (m_endIndex - m_startIndex + 1 - 1) * 2;
	}
}

////////////////////////////////
const SpriteDef& SpriteAnimationDef::GetFrameAtTime(float seconds) const
{
	int cycleDone = (int)(seconds / m_duration);
	int spriteIndex = -1;

	if (m_animationMode == SPRITE_ANIMATION_ONCE && cycleDone > 0) {
		spriteIndex = m_endIndex;
	} else {
		seconds -= m_duration * (float)cycleDone;
		int frame = (int)(seconds / GetDurationOfFrame(0));

		if (m_animationMode == SPRITE_ANIMATION_PINGPONG) {
			int baseFrames = m_endIndex - m_startIndex + 1;
			if (frame < baseFrames) {
				spriteIndex = frame + m_startIndex;
			} else {
				int backFromTheEnd = frame - baseFrames + 1;
				spriteIndex = m_endIndex - backFromTheEnd;
			}
		} else {
			spriteIndex = frame + m_startIndex;
		}
	}
	return m_spriteSheet.GetSpriteDef(spriteIndex);
}

////////////////////////////////
float SpriteAnimationDef::GetDurationOfFrame(int frameNumber) const
{
	UNUSED(frameNumber);
	return m_duration / (float)m_frameTotal;
}

#endif