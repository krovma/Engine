#pragma once
#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/Audio/AudioCommon.hpp"
#include <string>

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
	AudioChannel() = default;
	AudioChannel(const std::string& audioID, int loopTimes, bool startPaused);
	~AudioChannel();
public:
	FmodChannelID GetChannelID() const { return reinterpret_cast<FmodChannelID>(m_fmodChannel); }
	void Update(float deltaSecond);
	void UpdateChannelParameters(float deltaSecond);
	void Stop();
	//bool ShouldBeVirtualized();
	bool IsOneShot() const { return m_loopTimes == 0; }
	bool IsStopped() const { return m_state == STOP; }
	void SetVolume(float volume);
	void VolumeUp(float addValue);
	void VolumeDown(float substractValue);
	void SetPan(float pan);
	void SetSpeed(float speed);
	void SpeedUp(float addValue);
	void SpeedDown(float substractValue);

private:
	bool IsChannelValid() const;

private:
	FMOD::Channel* m_fmodChannel = nullptr;
	std::string m_audioID = "";
	State m_state = INIT;
	
	/// 0: One Shot; -1: Inf loop;
	int m_loopTimes = 0;
	bool m_stopRequested = false;
	bool m_startPaused = false;
	float m_channelSpeed = 1.f;
	float m_channelVolume = 1.f;
};