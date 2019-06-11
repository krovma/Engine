#include "Engine/Audio/AudioSubmix.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Audio/AudioChannel.hpp"
#include "Engine/Develop/DebugRenderer.hpp"
#include <algorithm>

AudioSubmix* g_audioMasterMix = nullptr;

////////////////////////////////
AudioSubmix::AudioSubmix(const std::string& id)
	:m_id(id)
{
}

////////////////////////////////
AudioSubmix::~AudioSubmix()
{
	m_fmodChannelGroup->release();
}

////////////////////////////////
void AudioSubmix::Update(float deltaSeconds)
{
	for (auto channelIter = m_channels.begin(); channelIter != m_channels.end();) {
		AudioChannelHandle channel = *channelIter;
		channel->Update(deltaSeconds);
		if (channel->IsStopped()) {
			//			delete channel;
			channelIter = m_channels.erase(channelIter);
		} else {
			++channelIter;
		}
	}
}

////////////////////////////////
void AudioSubmix::AddAudioChannel(AudioChannelHandle channel)
{
	m_channels.push_back(channel);
}

////////////////////////////////
void AudioSubmix::StopAll()
{
	for (auto channelIter = m_channels.begin(); channelIter != m_channels.end();) {
		//g_theAudio->StopChannel(*channelIter);
		(*channelIter)->Stop();
	}
}

////////////////////////////////
void AudioSubmix::SetVolume(float volume)
{
	m_fmodChannelGroup->setVolume(volume);
	m_channelVolume = volume;
}

////////////////////////////////
void AudioSubmix::VolumeUp(float addValue)
{
	SetVolume(addValue + m_channelVolume);
}

////////////////////////////////
void AudioSubmix::VolumeDown(float substractValue)
{
	SetVolume(std::max(0.f, m_channelVolume - substractValue));
}

////////////////////////////////
void AudioSubmix::SetPan(float pan)
{
	m_fmodChannelGroup->setPan(pan);
}

////////////////////////////////
void AudioSubmix::SetSpeed(float speed)
{
	for (auto channelIter = m_channels.begin(); channelIter != m_channels.end();) {
		(*channelIter)->SetSpeed(speed);
	}
}

////////////////////////////////
void AudioSubmix::SpeedUp(float addValue)
{
	for (auto channelIter = m_channels.begin(); channelIter != m_channels.end();) {
		(*channelIter)->SpeedUp(addValue);
	}
}

////////////////////////////////
void AudioSubmix::SpeedDown(float substractValue)
{
	for (auto channelIter = m_channels.begin(); channelIter != m_channels.end();) {
		(*channelIter)->SpeedDown(substractValue);
	}
}
