#pragma once

#include "Engine/Math/Vec3.hpp"
#include <memory>

#define AUDIO_MASTER_MIX "Master"

using FmodAssetID = size_t;
using FmodChannelID = size_t;
class AudioChannel;
using AudioChannelHandle = std::shared_ptr<AudioChannel>;
struct Vec3;
struct FMOD_VECTOR;

inline FMOD_VECTOR Vec3ToFMOD(const Vec3& vec);
inline float DBToVolume(float dB);
inline float VolumeToDB(float volume);