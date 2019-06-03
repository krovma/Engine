#pragma once

//-----------------------------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/Audio/AudioCommon.hpp"
#include "Engine/Audio/AudioAsset.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

//-----------------------------------------------------------------------------------------------
class AudioSystem;
class AudioChannel;
struct Vec3;

using AudioChannelHandle = std::shared_ptr<AudioChannel>;

/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
	friend AudioChannel;
public:
	AudioSystem();
	virtual ~AudioSystem();

	//#ToDo
	//static float DBToVolume(float dB);
	//static float VolumeToDB(float volume);

public:

	void BeginFrame();
	void Update(float deltaSecond);
	void EndFrame();


	AudioAsset AcquireAudio(const std::string& id, const std::string& path, bool is3dAudio=false);
	bool IsAudioLoaded(const std::string& id);
	bool IsAudioLoaded(FMOD::Sound* fmodSound);
	FmodAssetID GetFmodSound(const std::string& id);

	AudioChannelHandle PlayAudio(const std::string& id, bool isLooped=false, bool isPaused=false);
	//AudioChannel* PlayAudio3D(const std::string& id, const Vec3& position, bool isLooped=false, bool isPaused=false);
	void StopChannel(AudioChannelHandle channel );
	void SetChannelVolume(AudioChannelHandle channel, float volume );	// volume is in [0,1]
	void SetChannelPan(AudioChannelHandle channel, float pan );	// balance is in [-1,1], where 0 is L/R centered
	void SetChannelSpeed(AudioChannelHandle channel, float speed );		// speed is frequency multiplier (1.0 == normal)

	void ValidateResult( FMOD_RESULT result );
	
	std::vector<std::string> GetAllAudioAssetID() const;

protected:
	FMOD::System* m_fmod;
	std::map<std::string, AudioAsset> m_loadedAssets;
	std::vector<FMOD::Sound*> m_loadedFmodSounds;
	std::vector<AudioChannelHandle> m_channels;
};

extern AudioSystem* g_theAudio;

