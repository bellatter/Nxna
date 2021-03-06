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

		OemTilde,

		Space = 32,

		D0 = 48,
		D1 = 49,
		D2 = 50,
		D3 = 51,
		D4 = 52,
		D5 = 53,
		D6 = 54,
		D7 = 55,
		D8 = 56,
		D9 = 57,

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		F1 = 112,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,

		LeftShift = 160,
		RightShift = 161,
		LeftControl = 162,
		RightControl = 163,

		OemSemicolon = 186,
		OemPlus = 187,
		OemComma = 188,
		OemMinus = 189,
		OemPeriod = 190,
		OemQuestion = 191,

		OemOpenBrackets = 219,
		OemPipe = 220,
		OemCloseBrackets = 221,
		OemQuotes = 222,
		OemBackslash = 226
	END_NXNA_ENUM(Keys)
}
}

#endif // INPUT_KEYS_H
