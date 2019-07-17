#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Audio/AudioAsset.hpp"
#include "Engine/Audio/AudioChannel.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Audio/AudioSource.hpp"
#include "Engine/Audio/AudioScape.hpp"
#include "Game/EngineBuildPreferences.hpp"

#if !defined( ENGINE_DISABLE_AUDIO )
#if defined( _WIN64 ) && defined(_DEBUG)
#pragma comment( lib, "ThirdParty/fmod/fmodL64_vc.lib" )
#elif defined( _WIN64) && !defined(_DEBUG)
#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )
#endif

#include "Engine/Develop/DebugRenderer.hpp"
#define LOG_AUDIO_DEBUG

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

	g_audioMasterMix = new AudioSubmix(AUDIO_MASTER_MIX);
	m_fmod->getMasterChannelGroup(&(g_audioMasterMix->m_fmodChannelGroup));
	m_submixes[AUDIO_MASTER_MIX] = g_audioMasterMix;
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
void AudioSystem::Update(float deltaSeconds)
{
	for (auto& eachMix : m_submixes) {
		eachMix.second->Update(deltaSeconds);
	}
	for (auto& eachSource : m_sources) {
		eachSource->UpdateChannel3DParameters();
	}
	m_fmod->update();
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{

}


//-----------------------------------------------------------------------------------------------
std::string AudioSystem::AcquireAudio(const std::string& id, const std::string& path, bool is3dAudio/*=false*/)
{
	auto found = m_loadedAssets.find(id);
	if( found != m_loadedAssets.end() )
	{
		return found->second.m_id;
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
			//m_loadedFmodSounds.push_back(newSound);
			return newAsset.m_id;
		}
	}

	return AudioAsset::MISSING_AUDIO_ASSET.m_id;
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

////////////////////////////////
AudioChannelHandle AudioSystem::PlayAudio(const std::string& audioID, const std::string& submixID/*=AUDIO_MASTER_MIX*/, bool isLooped/*=false*/, bool startPaused/*=false*/)
{
	int loopCount = isLooped ? -1 : 0;
	//AudioChannel* channel = new AudioChannel(id, loopCount, startPaused);
	AudioChannelHandle channel = nullptr;
	auto foundSubmix = m_submixes.find(submixID);
	if (foundSubmix != m_submixes.end()) {
		channel = std::make_shared<AudioChannel>(audioID, foundSubmix->second, loopCount, startPaused);
		foundSubmix->second->AddAudioChannel(channel);
	} else {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log(Stringf("submix %s not exist", submixID.c_str()));
#endif
	}

	//m_channels.push_back(channel);
	return channel;
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::StopChannel(AudioChannelHandle channel)
{
	if (channel == nullptr) {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("Stopping a null channel");
#endif
		return;
	}

	channel->Stop();
}

void AudioSystem::SetChannelVolume(AudioChannelHandle channel, float volume)
{
	if (channel == nullptr) {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("Setting a null channel");
#endif
		return;
	}

	channel->SetVolume(volume);
}

void AudioSystem::SetChannelPan(AudioChannelHandle channel, float pan)
{
	if (channel == nullptr) {
#if defined(LOG_AUDIO_DEBUG)
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
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("Setting a null channel");
#endif
		return;
	}
	channel->SetSpeed(speed);
}


////////////////////////////////
AudioSubmix* AudioSystem::CreateSubmix(const std::string& id, const std::string& parentID)
{
	auto foundParentsubmix = m_submixes.find(parentID);
	if (foundParentsubmix != m_submixes.end()) {
		auto found = m_submixes.find(id);
		if (found == m_submixes.end()) {
			AudioSubmix* createdsubmix = new AudioSubmix(id);
			FMOD_RESULT result = m_fmod->createChannelGroup(id.c_str(), &(createdsubmix->m_fmodChannelGroup));
			ValidateResult(result);
			if (result == FMOD_OK) {
				foundParentsubmix->second->m_fmodChannelGroup->addGroup(createdsubmix->m_fmodChannelGroup);
				m_submixes[id] = createdsubmix;
				return createdsubmix;
			}
		} else {
#if defined(LOG_AUDIO_DEBUG)
			DebugRenderer::Log("Submix id already exist");
#endif
			return nullptr;
		}
	} else {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("Parent Submix id not exist");
#endif
		return nullptr;
	}
#if defined(LOG_AUDIO_DEBUG)
	DebugRenderer::Log("Failed to create submix");
#endif
	return nullptr;
}

////////////////////////////////
AudioSubmix* AudioSystem::GetSubmix(const std::string& id)
{
	auto found = m_submixes.find(id);
	if (found == m_submixes.end()) {
		return nullptr;
	} else {
		return found->second;
	}
}

////////////////////////////////
void AudioSystem::StopSubmix(AudioSubmix* submix)
{
	if (submix == nullptr) {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("Setting a null submix");
#endif
		return;
	}
	submix->StopAll();
}

////////////////////////////////
void AudioSystem::SetSubmixVolume(AudioSubmix* submix, float volume)
{
	if (submix == nullptr) {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("Setting a null submix");
#endif
		return;
	}
	submix->SetVolume(volume);
}

////////////////////////////////
void AudioSystem::SetSubmixPan(AudioSubmix* submix, float pan)
{
	if (submix == nullptr) {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("Setting a null submix");
#endif
		return;
	}
	submix->SetPan(pan);
}

////////////////////////////////
void AudioSystem::SetSubmixSpeed(AudioSubmix* submix, float speed)
{
	if (submix == nullptr) {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("Setting a null submix");
#endif
		return;
	}
	submix->SetSpeed(speed);
}

////////////////////////////////
FMOD::DSP* AudioSystem::CreateLPF()
{
	FMOD::DSP* created = nullptr;
	m_fmod->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &created);
	return created;
}

////////////////////////////////
AudioSource* AudioSystem::CreateAudioSource(const Vec3& position, float attenuationMinDist /*= 0.f*/, float attenuationMaxDist /*= 1.f*/, float volumeAtMin /*= 1.f*/)
{
	AudioSource* created = new AudioSource(position, attenuationMinDist, attenuationMaxDist, volumeAtMin);
	m_sources.push_back(created);
	return created;
}

////////////////////////////////
void AudioSystem::RemoveAudioSource(AudioSource* sourceToRemove)
{
	for (auto it = m_sources.begin(); it != m_sources.end(); ++it) {
		if (*it == sourceToRemove) {
			m_sources.erase(it);
			return;
		}
	}
}

////////////////////////////////
const AudioScape2D* AudioSystem::LoadScape(const char* filePath)
{
	if (m_scape != nullptr) {
		delete m_scape;
	}
	m_scape = new AudioScape2D(filePath);
	return m_scape;
}

////////////////////////////////
void AudioSystem::UnloadScape()
{
	delete m_scape;
	m_scape = nullptr;
}

////////////////////////////////
const AudioScape2D* AudioSystem::GetCurrentScape() const
{
	return m_scape;
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult(FMOD_RESULT result)
{
#if defined(LOG_AUDIO_DEBUG)
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
