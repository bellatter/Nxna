#include "TouchPanel.h"

namespace Nxna
{
namespace Input
{
namespace Touch
{
	std::vector<TouchLocation> TouchPanel::m_touches;

	std::vector<TouchLocation> TouchPanel::GetState()
	{
		return m_touches;
	}

	void TouchPanel::GetState(std::vector<TouchLocation>& state)
	{
		state.clear();
		state.reserve(m_touches.size());

		for (int i = 0; i < m_touches.size(); i++)
			state.push_back(m_touches[i]);
	}

	void TouchPanel::InjectFingerDown(int64_t id, int x, int y)
	{
		static int idTracker = 0;

		TouchLocation t;
		t.Id = idTracker++;
		t._internalID = id;
		t.Position.X = (float)x;
		t.Position.Y = (float)y;
		t.State = TouchLocationState_Pressed;

		m_touches.push_back(t);
	}

	void TouchPanel::InjectFingerMove(int64_t id, int x, int y)
	{
		// find the existing touch
		for (int i = 0; i < m_touches.size(); i++)
		{
			if (m_touches[i]._internalID == id)
			{
				m_touches[i].Position.X = (float)x;
				m_touches[i].Position.Y = (float)y;
				m_touches[i].State = TouchLocationState_Moved;

				break;
			}
		}
	}

	void TouchPanel::InjectFingerUp(int64_t id, int x, int y)
	{
		// find the existing touch
		for (int i = 0; i < m_touches.size(); i++)
		{
			if (m_touches[i]._internalID == id)
			{
				m_touches[i].Position.X = (float)x;
				m_touches[i].Position.Y = (float)y;
				m_touches[i].State = TouchLocationState_Released;

				break;
			}
		}
	}

	void TouchPanel::Refresh()
	{
		// remove all the released fingers
		for (int i = m_touches.size() - 1; i >= 0; i--)
		{
			if (m_touches[i].State == TouchLocationState_Released)
				m_touches.erase(m_touches.begin() + i);
		}
	}
}
}
}