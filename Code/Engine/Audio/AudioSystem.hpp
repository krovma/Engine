#pragma once

//-----------------------------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/Audio/AudioCommon.hpp"
#include "Engine/Audio/AudioAsset.hpp"
#include "Engine/Audio/AudioSubmix.hpp"
#include <string>
#include <vector>
#include <map>

//-----------------------------------------------------------------------------------------------
class AudioSystem;
class AudioChannel;
class AudioListener;
class AudioSource;
struct Vec3;

/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
	friend AudioChannel;
public:

	AudioSystem();
	virtual ~AudioSystem();

public:

	void BeginFrame();
	void Update(float deltaSeconds);
	void EndFrame();

	//////////////////////////////////////////////////////////////////////////
	//// Audio Assets
	//////////////////////////////////////////////////////////////////////////
	AudioAsset AcquireAudio(const std::string& id, const std::string& path, bool is3dAudio=false);
	bool IsAudioLoaded(const std::string& id);
	bool IsAudioLoaded(FMOD::Sound* fmodSound);
	FmodAssetID GetFmodSound(const std::string& id);
	//////////////////////////////////////////////////////////////////////////
	//// Channel
	//////////////////////////////////////////////////////////////////////////
	AudioChannelHandle PlayAudio(const std::string& audioID, const std::string& submixID = AUDIO_MASTER_MIX , bool isLooped=false, bool startPaused=false);
	void StopChannel(AudioChannelHandle channel);
	void SetChannelVolume(AudioChannelHandle channel, float volume);	// volume is in [0,1]
	void SetChannelPan(AudioChannelHandle channel, float pan);	// balance is in [-1,1], where 0 is L/R centered
	void SetChannelSpeed(AudioChannelHandle channel, float speed);		// speed is frequency multiplier (1.0 == normal)
	//////////////////////////////////////////////////////////////////////////
	//// Mix
	//////////////////////////////////////////////////////////////////////////
	AudioSubmix* CreateSubmix(const std::string& id, const std::string& parentID);
	AudioSubmix* GetSubmix(const std::string& id);
	void StopSubmix(AudioSubmix* channel);
	void SetSubmixVolume(AudioSubmix* channel, float volume);
	void SetSubmixPan(AudioSubmix* channel, float pan);
	void SetSubmixSpeed(AudioSubmix* channel, float speed);
	//////////////////////////////////////////////////////////////////////////
	//// 3D
	//////////////////////////////////////////////////////////////////////////
	AudioListener* GetListener() const { return m_currentListener; }
	void SetListener(AudioListener* listener) { m_currentListener = listener; }
	AudioSource* CreateAudioSource(const Vec3& position);
	void RemoveAudioSource(AudioSource* sourceToRemove);
	//////////////////////////////////////////////////////////////////////////
	//// Utilities
	//////////////////////////////////////////////////////////////////////////
	void ValidateResult( FMOD_RESULT result );
	std::vector<std::string> GetAllAudioAssetID() const;

protected:
	FMOD::System* m_fmod;
	std::map<std::string, AudioAsset> m_loadedAssets;
	//std::vector<FMOD::Sound*> m_loadedFmodSounds;
	std::map<std::string, AudioSubmix*> m_submixes;
	std::vector<AudioSource*> m_sources;
	AudioListener* m_currentListener = nullptr;
};

extern AudioSystem* g_theAudio;
extern AudioSubmix* g_audioMasterMix;
