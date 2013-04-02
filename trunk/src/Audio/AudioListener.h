#ifndef AUDIO_AUDIOLISTENER_H
#define AUDIO_AUDIOLISTENER_H

#include "../Vector3.h"

namespace Nxna
{
namespace Audio
{
	class AudioListener
	{
		Vector3 m_position;
		Vector3 m_forward;
		Vector3 m_up;
		Vector3 m_velocity;

	public:

		AudioListener()
		{
			m_up = Vector3::GetUp();
		}

		Vector3 GetPosition() const { return m_position; }
		void SetPosition(const Vector3& position) { m_position = position; }

		Vector3 GetForward() const { return m_forward; }
		void SetForward(const Vector3& forward) { m_forward = forward; }

		Vector3 GetUp() const { return m_up; }
		void SetUp(const Vector3& up) { m_up = up; }

		Vector3 GetVelocity() const { return m_velocity; }
		void SetVelocity(const Vector3& velocity) { m_velocity = velocity; }
	};
}
}

#endif // AUDIO_AUDIOLISTENER_H