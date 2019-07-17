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

////////////////////////////////
void AudioListener::GetNormedSpeakerPosition(Vec3* const pLeftSpeaker, Vec3* const pRightSpeaker) const
{
	//*pLeftSpeaker = (m_front - m_right).GetNormalized();
	//*pRightSpeaker = (m_front + m_right).GetNormalized();
	*pLeftSpeaker = -m_right;
	*pRightSpeaker = m_right;
}
