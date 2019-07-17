#pragma once
#include "Engine/Audio/AudioCommon.hpp"
#include <string>
#include <vector>

class AudioSubmix;
class AudioSource;

namespace FMOD
{
	class DSP;
	class Channel;
};

class AudioChannel
{
public:
	friend class AudioSystem;
	enum State
	{
		DEFAULT,
		INIT,
		LOAD,
		TOPLAY,
		PLAYING,
		STOPPING,
		VIRTUALIZING,
		VIRTUAL,
		STOP
	};

public:
	//////////////////////////////////////////////////////////////////////////
	///// Ctor Dtor Statics
	//////////////////////////////////////////////////////////////////////////
	AudioChannel() = default;
	AudioChannel(const std::string& audioID, AudioSubmix* mixer = nullptr, int loopTimes = 0, bool startPaused = false);
	~AudioChannel();

	//////////////////////////////////////////////////////////////////////////
	///// Update Functions
	//////////////////////////////////////////////////////////////////////////
	void Update(float deltaSecond);
	void UpdateChannelParameters(float deltaSecond);

	//////////////////////////////////////////////////////////////////////////
	///// Play Controls
	//////////////////////////////////////////////////////////////////////////
	void Stop();
	void SetVolume(float volume);
	void VolumeUp(float addValue);
	void VolumeDown(float substractValue);
	void SetPan(float pan);
	void SetSpeed(float speed);
	void SpeedUp(float addValue);
	void SpeedDown(float substractValue);

	//////////////////////////////////////////////////////////////////////////
	///// Mixing
	//////////////////////////////////////////////////////////////////////////

	void SetSubmix(const std::string& submixID);
	void SetSubmix(AudioSubmix* sumbix);

	void AddDSP(FMOD::DSP* dsp);

	void RemoveDSP(FMOD::DSP* dsp);

	//////////////////////////////////////////////////////////////////////////
	///// 3D
	//////////////////////////////////////////////////////////////////////////

	void BindToAudioSource(AudioSource* audioSource);

	//////////////////////////////////////////////////////////////////////////
	///// Inquiries
	//////////////////////////////////////////////////////////////////////////
	FmodChannelID GetChannelID() const { return reinterpret_cast<FmodChannelID>(m_fmodChannel); }
	//bool ShouldBeVirtualized();
	bool IsOneShot() const { return m_loopTimes == 0; }
	bool IsStopped() const { return m_state == STOP; }

private:
	bool IsChannelValid() const;

private:
	//////////////////////////////////////////////////////////////////////////
	///// System
	//////////////////////////////////////////////////////////////////////////
	FMOD::Channel* m_fmodChannel = nullptr;
	std::string m_audioID = ""; //#ToDo: Use `AudioAsset` instead;
	AudioSubmix* m_mixer;
	State m_state = INIT;
	std::vector<FMOD::DSP*> m_dspToAdd;
	//////////////////////////////////////////////////////////////////////////
	///// Playback
	//////////////////////////////////////////////////////////////////////////
	int m_loopTimes = 0;			//// 0: One Shot; -1: Inf loop;
	bool m_stopRequested = false;
	bool m_startPaused = false;
	float m_channelSpeed = 1.f;
	float m_channelVolume = 1.f;
	AudioSource* m_audioSource = nullptr;
};