#pragma once
#include "Engine/Audio/AudioCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>
class AudioSource;
class AudioChannel;
namespace FMOD
{
	class DSP;
}

struct AudioFalloffFunction
{
	/// An audio falloff function is a distance:float -> volume:float function
	float m_minDistance = 0.f;
	float m_maxDistance = 1.f;
	float m_volumeAtMin = 1.f;
	AudioFalloffFunction(float minDist = 0.f, float maxDist = 1.f, float volumeAtMin = 1.f)
		:m_minDistance(minDist), m_maxDistance(maxDist), m_volumeAtMin(volumeAtMin)
	{
	}
	virtual float GetVolumeAtDistance(float distance) = 0;
};

struct AudioFalloffLinear : public AudioFalloffFunction
{
	AudioFalloffLinear(float minDist = 0.f, float maxDist = 1.f, float volumeAtMin = 1.f)
		: AudioFalloffFunction(minDist, maxDist, volumeAtMin)
	{
	}
	virtual float GetVolumeAtDistance(float distance) override
	{
		if (distance < m_minDistance)
			return m_volumeAtMin;
		if (distance > m_maxDistance)
			return 0.f;
		return FloatMap(distance, m_minDistance, m_maxDistance, m_volumeAtMin, 0.f);
	}
};
struct AudioFalloffInvSquare : public AudioFalloffFunction
{
	AudioFalloffInvSquare(float minDist = 0.f, float maxDist = 1.f, float volumeAtMin = 1.f)
		: AudioFalloffFunction(minDist, maxDist, volumeAtMin)
	{
	}
	virtual float GetVolumeAtDistance(float distance) override {
		if (distance < m_minDistance)
			return m_volumeAtMin;
		if (distance > m_maxDistance)
			return 0.f;
		const float normedDist = FloatMap(distance, m_minDistance, m_maxDistance, 0.f, 1.f);
		const float normedDist2 = normedDist * normedDist;
		return FloatMap(normedDist2, 0.f, 1.f, m_volumeAtMin, 0.f);
	}
};

class AudioSource
{
	friend class AudioSystem;
	friend class AudioChannel;
public:
	void AddChannel(AudioChannelHandle channel);
	void RemoveChannel(AudioChannelHandle channel);
	void SetPosition(const Vec3& newPosition);
	Vec3 GetPosition() const { return m_position; }
	void BindNewFalloffFunction(AudioFalloffFunction* falloff);
private:
	AudioSource(const Vec3& position, float minDist = 0.f, float maxDist = 1.f, float volumeAtMin = 1.f);
	~AudioSource();
	void UpdateChannel3DParameters();
private:
	Vec3 m_position;
	AudioFalloffFunction* m_falloff = nullptr;
	std::vector<AudioChannelHandle> m_channels;
	FMOD::DSP* m_dspLPF = nullptr;
};