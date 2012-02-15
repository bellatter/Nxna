#ifndef NXNA_INPUT_TOUCH_TOUCHPANEL_H
#define NXNA_INPUT_TOUCH_TOUCHPANEL_H

#include <vector>
#include <cstdint>
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

	public:
		static std::vector<TouchLocation> GetState();
		static void GetState(std::vector<TouchLocation>& state);

		static void InjectFingerDown(int64_t id, int x, int y);
		static void InjectFingerMove(int64_t id, int x, int y);
		static void InjectFingerUp(int64_t id, int x, int y);

		// this should be called just before the Inject...() stuff begins happening
		static void Refresh();
	};
}
}
}

#endif // NXNA_INPUT_TOUCH_TOUCHPANEL_H
