#include "Engine/Audio/AudioListener.hpp"

////////////////////////////////
AudioListener::AudioListener(const Vec3& pos)
	:m_position(pos)
{

}

////////////////////////////////
void AudioListener::SetPosition(const Vec3& newPosition)
{
	m_position = newPosition;
}
