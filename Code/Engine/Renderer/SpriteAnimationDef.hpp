#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include <map>
enum SpriteAnimationMode
{
	SPRITE_ANIMATION_ONCE,
	SPRITE_ANIMATION_LOOP,
	//SPRITE_ANIMATION_PINGPONG,

	NUM_SPRITE_ANIMATION_TYPES
};

class SpriteAnimationDef;
struct SpriteAnimation;

struct SpriteAnimation
{
	SpriteAnimationDef* m_def=nullptr;
	int m_startCell = 0;
	int m_frameCount = 1;
	SpriteAnimationMode m_animationMode = SPRITE_ANIMATION_LOOP;
	float m_frameTime = 0.f;

	struct _FrameEvent
	{
		int m_frame=0;
		std::string m_action = "";
	};

	int GetFrameIndexAt(float time)
	{
		int frames = (int)(time / m_frameTime);
		return m_startCell + frames % m_frameCount;
	}

	std::vector<_FrameEvent> m_events;

};

class SpriteAnimationDef
{
public:
	static SpriteAnimationDef* AcquireAnimationDefFromFile(const std::string& path);
	SpriteAnimation* GetAnimaion(const std::string& id);
	SpriteSheet* GetSpriteSheet() { return m_spriteSheet; }
	AABB2 m_frameAABB;
	Vec2 m_pivot = Vec2(0.5f, 0.5f);
	Vec2 GetPivotOffset() const { static const Vec2 offset(m_pivot.x * -m_frameAABB.Max.x, m_pivot.y * -m_frameAABB.Max.y); return offset; }
	~SpriteAnimationDef();
private:
	static SpriteAnimationDef* LoadAnimationFromXml(const XmlElement& xml);
	static std::map<std::string, SpriteAnimationDef*> s_loadedAnimations;
private:
	std::map<std::string, SpriteAnimation*> m_animations;
	SpriteSheet* m_spriteSheet;
	IntVec2 m_frameSize;
	int m_ppu;
};