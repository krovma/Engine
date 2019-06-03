#pragma once
#include <string>
#include "Engine/Audio/AudioCommon.hpp"
constexpr size_t MISSING_AUDIO_ID = (FmodAssetID)(-1);
struct AudioAsset
{
	
public:
	static AudioAsset MISSING_AUDIO_ASSET;
	AudioAsset() = default;
public:
	std::string m_id = "";
	FmodAssetID m_fmodID = MISSING_AUDIO_ID;
	bool m_is3D = false;
};