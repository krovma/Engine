#pragma once
#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/Audio/AudioCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"

class AudioSubmix;
extern AudioSubmix* g_audioMasterMix;
class AudioSubmix
{
	friend class AudioSystem;
	friend class AudioChannel;
public:
	~AudioSubmix();
	void Update(float deltaSeconds);
	void AddAudioChannel(AudioChannelHandle channel);
	void StopAll();
	void SetVolume(float volume);
	void VolumeUp(float addValue);
	void VolumeDown(float substractValue);
	void SetPan(float pan);
	void SetSpeed(float speed);
	void SpeedUp(float addValue);
	void SpeedDown(float substractValue);
private:
	AudioSubmix(const std::string& id);
	std::string m_id;
	FMOD::ChannelGroup* m_fmodChannelGroup;
	std::vector<AudioChannelHandle> m_channels;
	float m_channelSpeed = 1.f;
	float m_channelVolume = 1.f;
};