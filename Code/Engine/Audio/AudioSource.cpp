#include "Engine/Audio/AudioSource.hpp"
#include "Engine/Audio/AudioChannel.hpp"
#include "Engine/Audio/AudioListener.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Audio/AudioScape.hpp"
#include "Engine/Math/Ray.hpp"
#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/Develop/DebugRenderer.hpp"
#include <algorithm>
//#define LOG_AUDIO_DEBUG

////////////////////////////////
AudioSource::AudioSource(const Vec3& position, float minDist /*= 0.f*/, float maxDist /*= 1.f*/, float volumeAtMin /*= 1.f*/)
	: m_position(position)
{
	m_falloff = new AudioFalloffLinear(minDist, maxDist, volumeAtMin);
	m_dspLPF = g_theAudio->CreateLPF();
}

////////////////////////////////
AudioSource::~AudioSource()
{
	m_dspLPF->release();
	delete m_falloff;
}

////////////////////////////////
void AudioSource::AddChannel(AudioChannelHandle channel)
{
	m_channels.push_back(channel);
	channel->AddDSP(m_dspLPF);
	//channel->
}

////////////////////////////////
void AudioSource::RemoveChannel(AudioChannelHandle channel)
{
	AudioChannel* target = channel.get();
	if (target == nullptr) {
		return;
	}
	for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
		if ((*it).get() == target) {
			(*it)->RemoveDSP(m_dspLPF);
			m_channels.erase(it);
			return;
		}
	}
}

////////////////////////////////
void AudioSource::SetPosition(const Vec3& newPosition)
{
	m_position = newPosition;
}

////////////////////////////////
void AudioSource::UpdateChannel3DParameters()
{
	static IntVec2 __go9[9] = {
		IntVec2(0,0),
		IntVec2(0, 1),
		IntVec2(1, 0),
		IntVec2(-1, 0),
		IntVec2(0, -1),
		IntVec2(1, 1),
		IntVec2(1, -1),
		IntVec2(-1, 1),
		IntVec2(-1, -1),

	};
	const AudioListener* listener = g_theAudio->GetListener();
	Vec3 listenerPos = listener->GetPosition();
	Vec3 listenerFront = listener->GetFront();
	Vec3 lSpeaker;
	Vec3 rSpeaker;
	listener->GetNormedSpeakerPosition(&lSpeaker, &rSpeaker);
	IntVec2 iSoundPos = IntVec2((int)m_position.x, (int)m_position.z);
	Vec2 soundPos2 = Vec2(m_position.x, m_position.z);
	IntVec2 iListenerPos = IntVec2((int)listenerPos.x, (int)listenerPos.z);
	float volumeRolloff = 1.f;

	const AudioScape2D* scape = g_theAudio->GetCurrentScape();

	if (listener == nullptr) {
#if defined(LOG_AUDIO_DEBUG)
		DebugRenderer::Log("No Listener designated!!", 5.f, Rgba::RED);
#endif
		return;
	}

	Vec2 listenerPos2 = Vec2(listenerPos.x, listenerPos.z);
	Vec2 virtualSoundPos;
	float volume = 0;
	float rolloff = 1.f;
	int nAudiable = 0;
	float fWeightTotal = 0.f;
	int nDirect = 0;
	Vec2 blenderDisp = listenerPos2 - (Vec2(iListenerPos) + Vec2::ONE * 0.5f);

	float centerVolume = 0;
	Vec2 centerPos;
	{
		IntVec2 currentTileCoord = iListenerPos + __go9[0];
		auto path = scape->GetPath(iSoundPos, currentTileCoord);
		if (path.IsAudible()){
			if (path.IsBlocked()) {
				rolloff *= 0.75;
				centerVolume = m_falloff->GetVolumeAtDistance(path.m_distance);
				centerPos = (Vec2(path.m_dir) + Vec2::ONE * 0.5f);
			} else {
				Vec2 currentTile = Vec2(currentTileCoord) + Vec2::ONE * 0.5f;
				centerVolume = m_falloff->GetVolumeAtDistance(GetDistance(currentTile, soundPos2));
				centerPos += soundPos2;
			}
		}
	}
	for (int i = 1; i < 9; ++i) {
		IntVec2 currentTileCoord = iListenerPos + __go9[i];
		if (scape->IsCoordValid(currentTileCoord.x, currentTileCoord.y)) {
			auto path = scape->GetPath(iSoundPos, currentTileCoord);
			float blendFactor;
			{
				Vec2 go9i2 = Vec2(__go9[i]);
				blendFactor = Clamp(blenderDisp.DotProduct(go9i2) / go9i2.GetLength(), 0.0f, 0.5f);
			}
			DebugRenderer::Log(Stringf("To <%d %d> factor = %g", currentTileCoord.x, currentTileCoord.y, blendFactor), 0);
			if (blendFactor == 0) {
				continue;
			}
			if (path.IsAudible()) {
				++nAudiable;
				fWeightTotal += 1;
				if (path.IsBlocked()) {
					volume += Lerp(centerVolume, m_falloff->GetVolumeAtDistance(path.m_distance), blendFactor);
					virtualSoundPos += Lerp(centerPos, (Vec2(path.m_dir) + Vec2::ONE * 0.5f), blendFactor);
					rolloff *= ::powf(path.GetFrequencyRolloff(), blendFactor);
				} else {
					Vec2 currentTile = Vec2(currentTileCoord) + Vec2::ONE * 0.5f;
					volume += Lerp(centerVolume, m_falloff->GetVolumeAtDistance(GetDistance(currentTile, soundPos2)), blendFactor);
					virtualSoundPos += Lerp(centerPos, soundPos2, blendFactor);
				}
			} else {
				//
			}
		}
	}
	if (nAudiable != 0 && fWeightTotal != 0.f) {
		volume /= (float)nAudiable;
		virtualSoundPos /= fWeightTotal;
	}

	DebugRenderer::Log(Stringf("<%d %d> volume is %g at <%g %g>", iSoundPos.x, iSoundPos.y, volume, virtualSoundPos.x, virtualSoundPos.y), 0.f);

// 	Vec2 sum = Vec2::ZERO;
// 	int nAudiable = 9;
// 	auto blenderDisp = Vec2(listenerPos.x, listenerPos.z) - Vec2(m_position.x, m_position.z);
// 	for (int i = 0; i < 9; ++i) {
// 		IntVec2 adjPos = iListenerPos + __go4[i];
// 		if (scape->IsCoordValid(adjPos.x, adjPos.y)) {
// 			auto adjPath = scape->GetPath(iSoundPos, adjPos);
// 			Vec2 adjVirtualSource = Vec2(iSoundPos) + Vec2::ONE * 0.5f;
// 			if (adjPath.IsBlocked()) {
// 				adjVirtualSource = Vec2(adjPath.m_dir) + Vec2::ONE * 0.5f;
// 			}
// 			DebugRenderer::DrawPoint3D(Vec3(adjVirtualSource.x, 0, adjVirtualSource.y), 0.25f, 0.f, Rgba::MAROON);
// 			float blendK = (Vec2(adjPos - iListenerPos).GetNormalized().DotProduct(blenderDisp));
// 			Vec2 blended = Lerp(centerVPos, adjVirtualSource, blendK);
// 			DebugRenderer::DrawPoint3D(Vec3(blended.x, 0, blended.y), 0.25f, 0.f, Rgba::RED);
// 			DebugRenderer::Log(Stringf("%f 0 %f", blended.x, blended.y), 0.f);
// 			sum += blended;
// 			//++nAudiable;
// 		}
// 	}
// 	virtualPos = sum / nAudiable;

	Vec3 virtualPos3 = Vec3(virtualSoundPos.x, 0, virtualSoundPos.y);
	Vec3 disp = virtualPos3 - listener->GetPosition();
	disp.Normalize();
	//float pan = disp.DotProduct(listener->GetRight().GetNormalized());
	const float gainL = FloatMap(lSpeaker.DotProduct(disp), -1.f, 1.f, 0.f, 1.f);
	const float gainR = FloatMap(rSpeaker.DotProduct(disp), -1.f, 1.f, 0.f, 1.f);
	const float pan = FloatMap(gainR / sqrtf(gainR + gainL), 0.f, 1.f, -1.f, 1.f);
	const float backward = disp.DotProduct(listenerFront);
	
	if (backward < 0.f || rolloff != 1.f) {
		m_dspLPF->setBypass(false);
		if (backward < 0.f) {
			m_dspLPF->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, FloatMap(backward * backward * backward * rolloff, 0, -1.f, 22050.f, 3840.f));
		} else {
			m_dspLPF->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, FloatMap(rolloff, 0, -1.f, 22050.f, 3840.f));
		}
	} else {
		m_dspLPF->setBypass(true);
	}
	for (auto it = m_channels.begin(); it != m_channels.end();) {
		AudioChannelHandle channel = *it;
		if (channel->IsStopped()) {
			it = m_channels.erase(it);
			continue;
		}
			channel->SetPan(pan);
			channel->SetVolume(volume * volumeRolloff);
		++it;
	}
}

////////////////////////////////
void AudioSource::BindNewFalloffFunction(AudioFalloffFunction* falloff)
{
	delete m_falloff;
	m_falloff = falloff;
}
