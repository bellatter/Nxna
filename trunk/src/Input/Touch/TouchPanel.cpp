#include "TouchPanel.h"

namespace Nxna
{
namespace Input
{
namespace Touch
{
	std::vector<TouchPanel::InternalTouchLocation> TouchPanel::m_touches;
	int TouchPanel::m_width, TouchPanel::m_height;

	std::vector<TouchLocation> TouchPanel::GetState()
	{
		std::vector<TouchLocation> touches;
		GetState(touches);

		return touches;
	}

	void TouchPanel::GetState(std::vector<TouchLocation>& state)
	{
		state.clear();
		state.reserve(m_touches.size());

		for (int i = 0; i < m_touches.size(); i++)
		{
			// ignore released touch points that have already been read
			if (m_touches[i].Current.State == TouchLocationState::Released &&
				m_touches[i].Previous.State == TouchLocationState::Released)
				continue;

			state.push_back(m_touches[i].Current);

			if (m_touches[i].Current.State == TouchLocationState::Pressed)
			{
				// convert read Pressed touchpoints to Moved
				m_touches[i].Current.State = TouchLocationState::Moved;
				m_touches[i].Previous.State = TouchLocationState::Pressed;
			}
			else if (m_touches[i].Current.State == TouchLocationState::Released)
			{
				m_touches[i].Previous.State = TouchLocationState::Released;
			}
		}

		// remove all the released fingers
		for (int i = m_touches.size() - 1; i >= 0; i--)
		{
			if (m_touches[i].Current.State == TouchLocationState::Released)
				m_touches.erase(m_touches.begin() + i);
		}
	}

	void TouchPanel::InjectFingerDown(int64_t id, float x, float y)
	{
		static int idTracker = 0;

		InternalTouchLocation t;
		t.Current.Id = idTracker++;
		t.Current._internalID = id;
		t.Current.Position.X = x * m_width;
		t.Current.Position.Y = y * m_height;
		t.Current.State = TouchLocationState::Pressed;
		t.Previous.State = TouchLocationState::Invalid;

		m_touches.push_back(t);
	}

	void TouchPanel::InjectFingerMove(int64_t id, float x, float y)
	{
		// find the existing touch
		for (int i = 0; i < m_touches.size(); i++)
		{
			if (m_touches[i].Current._internalID == id)
			{
				m_touches[i].Current.Position.X = x * m_width;
				m_touches[i].Current.Position.Y = y * m_height;

				if (m_touches[i].Current.State != TouchLocationState::Invalid)
					m_touches[i].Current.State = TouchLocationState::Moved;

				break;
			}
		}
	}

	void TouchPanel::InjectFingerUp(int64_t id, float x, float y)
	{
		// find the existing touch
		for (int i = 0; i < m_touches.size(); i++)
		{
			if (m_touches[i].Current._internalID == id)
			{
				m_touches[i].Current.Position.X = x * m_width;
				m_touches[i].Current.Position.Y = y * m_height;
				m_touches[i].Current.State = TouchLocationState::Released;

				break;
			}
		}
	}

	void TouchPanel::Refresh()
	{
	}
}
}
}