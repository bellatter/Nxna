#ifndef NXNA_INPUT_TOUCH_TOUCHPANEL_H
#define NXNA_INPUT_TOUCH_TOUCHPANEL_H

#include <vector>
#include <stdint.h>
#include "../../Vector2.h"

namespace Nxna
{
namespace Input
{
namespace Touch
{
	enum TouchLocationState
	{
		TouchLocationState_Invalid,
		TouchLocationState_Released,
		TouchLocationState_Pressed,
		TouchLocationState_Moved
	};

	struct TouchLocation
	{
		int Id;
		Vector2 Position;
		TouchLocationState State;

		int64_t _internalID;
	};

	class TouchPanel
	{
		static std::vector<TouchLocation> m_touches;
		static int m_width, m_height;

	public:

		static int GetDisplayWidth() { return m_width; }
		static void SetDisplayWidth(int width) { m_width = width; }

		static int GetDisplayHeight() { return m_height; }
		static void SetDisplayHeight(int height) { m_height = height; }

		static std::vector<TouchLocation> GetState();
		static void GetState(std::vector<TouchLocation>& state);

		static void InjectFingerDown(int64_t id, float x, float y);
		static void InjectFingerMove(int64_t id, float x, float);
		static void InjectFingerUp(int64_t id, float x, float y);

		// this should be called just before the Inject...() stuff begins happening
		static void Refresh();
	};
}
}
}

#endif // NXNA_INPUT_TOUCH_TOUCHPANEL_H
