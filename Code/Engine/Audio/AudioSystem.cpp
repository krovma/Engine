#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Audio/AudioAsset.hpp"
#include "Engine/Audio/AudioChannel.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"

#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_AUDIO )
#if defined( _WIN64 ) && defined(_DEBUG)
#pragma comment( lib, "ThirdParty/fmod/fmodL64_vc.lib" )
#elif defined( _WIN64) && !defined(_DEBUG)
#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )
#endif

#if defined(_DEBUG)
#include "Engine/Develop/DebugRenderer.hpp"
#endif

AudioSystem* g_theAudio = nullptr;

//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
	: m_fmod( nullptr )
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmod );
	ValidateResult( result );

	result = m_fmod->init( 512, FMOD_INIT_3D_RIGHTHANDED, nullptr );
	ValidateResult( result );
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	m_fmod->release();
	m_fmod = nullptr;
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{

}


////////////////////////////////
void AudioSystem::Update(float deltaSecond)
{
	for (auto channelIter = m_channels.begin(); channelIter != m_channels.end();) {
		
		AudioChannelHandle channel = *channelIter;
		channel->Update(deltaSecond);
		if (channel->IsStopped()) {
//			delete channel;
			channelIter = m_channels.erase(channelIter);
		} else {
			++channelIter;
		}
	}
	m_fmod->update();
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{

}


//-----------------------------------------------------------------------------------------------
AudioAsset AudioSystem::AcquireAudio(const std::string& id, const std::string& path, bool is3dAudio/*=false*/)
{
	auto found = m_loadedAssets.find(id);
	if( found != m_loadedAssets.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		AudioAsset newAsset;
		newAsset.m_id = id;
		newAsset.m_is3D = is3dAudio;
		if (is3dAudio) {
			m_fmod->createSound(path.c_str(), FMOD_3D, nullptr, &newSound);
		} else {
			m_fmod->createSound(path.c_str(), FMOD_2D, nullptr, &newSound);
		}
		if( newSound )
		{
			newAsset.m_fmodID = reinterpret_cast<FmodAssetID>(newSound);
			m_loadedAssets[id] = newAsset;
			m_loadedFmodSounds.push_back(newSound);
			return newAsset;
		}
	}

	return AudioAsset::MISSING_AUDIO_ASSET;
}


////////////////////////////////
bool AudioSystem::IsAudioLoaded(const std::string& id)
{
	auto found = m_loadedAssets.find(id);
	if (found != m_loadedAssets.end()) {
		return true;
	}
	return false;
}

////////////////////////////////
bool AudioSystem::IsAudioLoaded(FMOD::Sound* fmodSound)
{
	UNUSED(fmodSound);
	return true;
}

////////////////////////////////
FmodAssetID AudioSystem::GetFmodSound(const std::string& id)
{
	return m_loadedAssets[id].m_fmodID;
}

//-----------------------------------------------------------------------------------------------
AudioChannelHandle AudioSystem::PlayAudio(const std::string& id, bool isLooped/*=false*/, bool startPaused/*=false*/)
{

	int loopCount = isLooped ? -1 : 0;
	//AudioChannel* channel = new AudioChannel(id, loopCount, startPaused);
	auto channel = std::make_shared<AudioChannel>(id, loopCount, startPaused);
	m_channels.push_back(channel);
	return channel;
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::StopChannel(AudioChannelHandle channel)
{
	if (channel == nullptr) {
#if defined(_DEBUG)
		DebugRenderer::Log("Stopping a null channel");
#endif
		return;
	}

	channel->Stop();
}

void AudioSystem::SetChannelVolume(AudioChannelHandle channel, float volume)
{
	if (channel == nullptr) {
#if defined(_DEBUG)
		DebugRenderer::Log("Setting a null channel");
#endif
		return;
	}

	channel->SetVolume(volume);
}

void AudioSystem::SetChannelPan(AudioChannelHandle channel, float pan)
{
	if (channel == nullptr) {
#if defined(_DEBUG)
		DebugRenderer::Log("Setting a null channel");
#endif
		return;
	}
	channel->SetPan(pan);
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetChannelSpeed(AudioChannelHandle channel, float speed)
{
	if (channel == nullptr) {
#if defined(_DEBUG)
		DebugRenderer::Log("Setting a null channel");
#endif
		return;
	}
	channel->SetSpeed(speed);
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult(FMOD_RESULT result)
{
#if defined(_DEBUG)
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
#endif
}


////////////////////////////////
std::vector<std::string> AudioSystem::GetAllAudioAssetID() const
{
	std::vector<std::string> allID;
	for (auto each = m_loadedAssets.begin(); each != m_loadedAssets.end(); ++each) {
		allID.emplace_back(each->first);
	}
	return allID; //RVO
}

#endif // !defined( ENGINE_DISABLE_AUDIO )
