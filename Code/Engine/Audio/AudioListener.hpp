#pragma once
#include "Engine/Audio/AudioCommon.hpp"

class AudioListener;

class AudioListener
{
	friend class AudioSystem;
public:
	AudioListener() = default;
	AudioListener(const Vec3& pos);

	void SetPosition(const Vec3& newPosition);
	Vec3 GetPosition() const { return m_position; }
	void SetRight(const Vec3& right) { m_right = right; }
	Vec3 GetRight() const { return m_right; }
private:
	Vec3 m_position = Vec3::ZERO;
	Vec3 m_right = Vec3::ONE;
};