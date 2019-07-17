#pragma once

//-----------------------------------------------------------------------------------------------
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
class AudioScape2D;
enum FMOD_RESULT :int;
struct Vec3;
namespace FMOD
{
	class DSP;
	class Sound;
	class System;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
	friend AudioChannel;
	friend AudioSubmix;
	friend AudioSource;
	friend AudioListener;
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
	std::string AcquireAudio(const std::string& audioID, const std::string& path, bool is3dAudio=false);
	bool IsAudioLoaded(const std::string& audioID);
	//////////////////////////////////////////////////////////////////////////
	//// Channel
	//////////////////////////////////////////////////////////////////////////
//	AudioChannelHandle PlayAudio(const AudioAsset& audioAsset, AudioSubmix* submix = nullptr, bool isLooped = false, bool startPaused = false);
	AudioChannelHandle PlayAudio(const std::string& audioID, const std::string& submixID = AUDIO_MASTER_MIX , bool isLooped=false, bool startPaused=false);
	void StopChannel(AudioChannelHandle channel);
	void SetChannelVolume(AudioChannelHandle channel, float volume);	// volume is in [0,1]
	void SetChannelPan(AudioChannelHandle channel, float pan);	// pan is in [-1,1], where 0 is L/R centered
	void SetChannelSpeed(AudioChannelHandle channel, float speed);		// speed is frequency multiplier (1.0 == normal)
	//////////////////////////////////////////////////////////////////////////
	//// Mix
	//////////////////////////////////////////////////////////////////////////
	AudioSubmix* CreateSubmix(const std::string& submixID, const std::string& parentID);
	AudioSubmix* GetSubmix(const std::string& submixID);
	void StopSubmix(AudioSubmix* channel);
	void SetSubmixVolume(AudioSubmix* channel, float volume);
	void SetSubmixPan(AudioSubmix* channel, float pan);
	void SetSubmixSpeed(AudioSubmix* channel, float speed);
	//////////////////////////////////////////////////////////////////////////
	//// 3D
	//////////////////////////////////////////////////////////////////////////
	AudioListener* GetListener() const { return m_currentListener; }
	void SetListener(AudioListener* listener) { m_currentListener = listener; }
	AudioSource* CreateAudioSource(const Vec3& position, float attenuationMinDist = 0.f, float attenuationMaxDist = 1.f, float volumeAtMin = 1.f);
	void RemoveAudioSource(AudioSource* sourceToRemove);

	const AudioScape2D* LoadScape(const char* mapFilePath);
	void UnloadScape();
	const AudioScape2D* GetCurrentScape() const;

	//////////////////////////////////////////////////////////////////////////
	//// Utilities
	//////////////////////////////////////////////////////////////////////////
	std::vector<std::string> GetAllAudioAssetID() const;
	//Vec3 GetBlocker();
private:
	FmodAssetID GetFmodSound(const std::string& audioID);
	bool IsAudioLoaded(FMOD::Sound* fmodSound);
	void ValidateResult(FMOD_RESULT result);
	FMOD::DSP* CreateLPF();
protected:
	FMOD::System* m_fmod;
	std::map<std::string, AudioAsset> m_loadedAssets;
	//std::vector<FMOD::Sound*> m_loadedFmodSounds;
	std::map<std::string, AudioSubmix*> m_submixes;
	std::vector<AudioSource*> m_sources;
	AudioListener* m_currentListener = nullptr;
	AudioScape2D* m_scape = nullptr;
};

extern AudioSystem* g_theAudio;
extern AudioSubmix* g_audioMasterMix;
