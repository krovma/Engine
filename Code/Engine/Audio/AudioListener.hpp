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
	void SetOrientation(const Vec3& front, const Vec3& right) {
		SetFront(front);
		SetRight(right);
	}
	void GetNormedSpeakerPosition(Vec3* const pLeftSpeaker, Vec3* const pRightSpeaker) const;
	Vec3 GetRight() const { return m_right; }
	Vec3 GetFront() const { return m_front; }
private:
	void SetRight(const Vec3& right) { m_right = right; m_right.Normalize(); }
	void SetFront(const Vec3& front) { m_front = front; m_front.Normalize(); }
private:
	Vec3 m_position = Vec3::ZERO;
	Vec3 m_right = Vec3(1, 0, 0);
	Vec3 m_front = Vec3(0, 0, -1);
};