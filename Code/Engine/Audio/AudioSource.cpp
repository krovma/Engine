#include "Engine/Audio/AudioSource.hpp"
#include "Engine/Audio/AudioChannel.hpp"
#include "Engine/Audio/AudioListener.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include <algorithm>
#if defined(_DEBUG)
#include "Engine/Develop/DebugRenderer.hpp"
#endif
////////////////////////////////
AudioSource::AudioSource(const Vec3& position)
	: m_position(position)
{

}

////////////////////////////////
void AudioSource::AddChannel(AudioChannelHandle channel)
{
	m_channels.push_back(channel);
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
	const AudioListener* listener = g_theAudio->GetListener();
	if (listener == nullptr) {
#if defined(_DEBUG)
		DebugRenderer::Log("No Listener designated!!", 5.f, Rgba::RED);
#endif
	}
	Vec3 disp = m_position - listener->GetPosition();
	//#ToDo: change me
	float distanceInv = 1.f / std::max(disp.GetLength(), 0.01f);
	float volume = std::min(2.f, 5.f * distanceInv * distanceInv);
	disp.Normalize();
	float pan = disp.DotProduct(listener->GetRight().GetNormalized());

	for (auto it = m_channels.begin(); it != m_channels.end();) {
		AudioChannelHandle channel = *it;
		if (channel->IsStopped()) {
			it = m_channels.erase(it);
			continue;
		}
		channel->SetPan(pan);
		channel->SetVolume(volume);
#if defined(_DEBUG)
		DebugRenderer::Log(Stringf("panning to %g volume to %g", pan, volume), 0.f);
#endif
		++it;
	}
}
