#ifndef INPUT_KEYS_H
#define INPUT_KEYS_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Input
{
	NXNA_ENUM(Keys)
		None,
		Back,
		Tab,
		Enter,

		Left,
		Up,
		Right,
		Down,

		Escape = 27,

		OemTilde
	END_NXNA_ENUM(Keys)
}
}

#endif // INPUT_KEYS_H
