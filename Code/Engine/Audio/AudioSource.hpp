#pragma once
#include "Engine/Audio/AudioCommon.hpp"
#include <vector>
class AudioSource;
class AudioChannel;

class AudioSource
{
	friend class AudioSystem;
	friend class AudioChannel;
private:
	AudioSource(const Vec3& position);
public:
	void AddChannel(AudioChannelHandle channel);
	void RemoveChannel(AudioChannelHandle channel);
	void SetPosition(const Vec3& newPosition);
	Vec3 GetPosition() const { return m_position; }
	void UpdateChannel3DParameters();
private:
	Vec3 m_position;
	std::vector<AudioChannelHandle> m_channels;
};