#include "Engine/Audio/AudioChannel.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Audio/AudioSource.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <algorithm>
#if !defined( ENGINE_DISABLE_AUDIO )
#if defined(_DEBUG)
#include "Engine/Develop/DebugRenderer.hpp"
#endif

////////////////////////////////
AudioChannel::AudioChannel(const std::string& audioID, AudioSubmix* mixer /*= nullptr*/, int loopTimes /*= 0*/, bool startPaused /*= false*/)
	: m_audioID(audioID)
	, m_loopTimes(loopTimes)
	, m_state(INIT)
	, m_startPaused(startPaused)
{
	if (mixer == nullptr) {
		m_mixer = g_audioMasterMix;
	} else {
		m_mixer = mixer;
	}
}

////////////////////////////////
AudioChannel::~AudioChannel()
{
	//DebugRenderer::Log("Delete!");
}

////////////////////////////////
void AudioChannel::Update(float deltaSecond)
{
	switch (m_state) {
	case DEFAULT:
		break;
	case INIT:
	{
		m_state = TOPLAY;
		break;
	}
	case LOAD:
	{
		if (g_theAudio->IsAudioLoaded(m_audioID)) {
			m_state = TOPLAY;
		} else {
			m_state = STOPPING;
		}
		break;
	}
	case TOPLAY:
	{
		if (m_stopRequested) {
			m_state = STOPPING;
			return;
		}
		if (g_theAudio->IsAudioLoaded(m_audioID)) {
			m_fmodChannel = nullptr;
			auto sound = reinterpret_cast<FMOD::Sound*>(g_theAudio->GetFmodSound(m_audioID));
			g_theAudio->m_fmod->playSound(sound, m_mixer->m_fmodChannelGroup, m_startPaused, &m_fmodChannel);
			if (!m_fmodChannel) {
				m_state = STOPPING;
			} else {
				unsigned int playbackMode = IsOneShot() ? FMOD_LOOP_OFF : FMOD_LOOP_NORMAL;
				m_fmodChannel->setMode(playbackMode);
				m_fmodChannel->setLoopCount(m_loopTimes);
				m_state = PLAYING;
			}
		} else {
			m_state = LOAD;
			return;
		}
		break;
	}
	case PLAYING:
	{
		// #Todo: Virtualizing
		UpdateChannelParameters(deltaSecond);
		bool playing;
		m_fmodChannel->isPlaying(&playing);
		if (m_stopRequested || !playing) {
			m_state = STOPPING;
			return;
		}
		break;
	}
	case STOPPING:
	{
		// #Todo: fade out
		UpdateChannelParameters(deltaSecond);
		bool playing;
		m_fmodChannel->isPlaying(&playing);
		if (!playing) {
			m_state = STOP;
			return;
		}
		break;
	}
	case VIRTUALIZING:
		break;
	case VIRTUAL:
		break;
	case STOP:
		break;
	}
}

////////////////////////////////
void AudioChannel::UpdateChannelParameters(float deltaSecond)
{
	//#Todo: do acoustics
	UNUSED(deltaSecond);
}

////////////////////////////////
void AudioChannel::Stop()
{
	m_stopRequested = true;
}


////////////////////////////////
void AudioChannel::SetVolume(float volume)
{
	if (IsChannelValid()) {
		m_fmodChannel->setVolume(volume);
		m_channelVolume = volume;
	}
}

////////////////////////////////
void AudioChannel::VolumeUp(float addValue)
{
	SetVolume(addValue + m_channelVolume);
}

////////////////////////////////
void AudioChannel::VolumeDown(float substractValue)
{
	SetVolume(std::max(0.f, m_channelVolume - substractValue));
}

////////////////////////////////
void AudioChannel::SetPan(float pan)
{
	if (IsChannelValid()) {
		m_fmodChannel->setPan(pan);
	}
}

////////////////////////////////
void AudioChannel::SetSpeed(float speed)
{
	if (!IsChannelValid()) {
		return;
	}
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	m_fmodChannel->getCurrentSound(&currentSound);
	if (!currentSound) {
#if defined(_DEBUG)
		DebugRenderer::Log(Stringf("Cannot get fmod sound from channel %i", m_fmodChannel), 10, Rgba::RED);
#endif
		return;
	}
	int priority = 0;
	currentSound->getDefaults(&frequency, &priority);
	m_fmodChannel->setFrequency(frequency * speed);
	m_channelSpeed = speed;
	DebugRenderer::Log(Stringf("Channel %i speed set to %g", m_fmodChannel, m_channelSpeed));
}

////////////////////////////////
void AudioChannel::SpeedUp(float addValue)
{
	SetSpeed(addValue + m_channelSpeed);
}

////////////////////////////////
void AudioChannel::SpeedDown(float substractValue)
{
	SetSpeed(std::max(0.f, m_channelSpeed - substractValue));
}

////////////////////////////////
void AudioChannel::BindToAudioSource(AudioSource* audioSource)
{
	auto handle = std::shared_ptr<AudioChannel>(this);
	if (m_audioSource != nullptr) {
		m_audioSource->RemoveChannel(handle);
	}
	m_audioSource = audioSource;
	if (audioSource) {
		m_audioSource->AddChannel(handle);
	}
}

////////////////////////////////
bool AudioChannel::IsChannelValid() const
{
	return m_fmodChannel != nullptr && m_state != STOP && m_state != INIT && m_state != LOAD;
}

#endif //DISABLE_AUDIO