#ifndef NXNA_INPUT_TOUCH_TOUCHPANEL_H
#define NXNA_INPUT_TOUCH_TOUCHPANEL_H

#include <vector>
#include <stdint.h>
#include "../../NxnaConfig.h"
#include "../../Vector2.h"

namespace Nxna
{
namespace Input
{
namespace Touch
{
	NXNA_ENUM(TouchLocationState)
		Invalid,
		Released,
		Pressed,
		Moved
	END_NXNA_ENUM(TouchLocationState)

	struct TouchLocation
	{
		int Id;
		Vector2 Position;
		TouchLocationState State;

		int64_t _internalID;
	};

	class TouchPanel
	{
		struct InternalTouchLocation
		{
			TouchLocation Current;
			TouchLocation Previous;
		};

		static std::vector<InternalTouchLocation> m_touches;
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
