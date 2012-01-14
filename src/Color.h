#ifndef COLOR_H
#define COLOR_H

#include "NxnaConfig.h"

namespace Nxna
{
struct Color
{
	Color()
	{
		R = G = B = A = 0;
	}

	Color(int r, int g, int b)
	{
		R = (byte)r; G = (byte)g; B = (byte)b; A = 255;
	}

	Color(int r, int g, int b, int a)
	{
		R = (byte)r; G = (byte)g; B = (byte)b; A = (byte)a;
	}

	byte R;
	byte G;
	byte B;
	byte A;

	Color operator *(float s)
	{
		Color r;
		r.R = (byte)(R * s);
		r.G = (byte)(G * s);
		r.B = (byte)(B * s);
		r.A = (byte)(A * s);

		return r;
	}

	static Color GetWhite() { return Color(255, 255, 255); }
	static Color GetBlack() { return Color(0, 0, 0); }
	static Color GetGray() { return Color(128, 128, 128); }
	static Color GetCornflowerBlue() { return Color(100, 149, 237); }
	static Color GetGoldenrod() { return Color(218, 165, 32); }
	static Color GetRed() { return Color(255, 0, 0); }
};

static bool operator==(const Color& c1, const Color& c2)
{
	return c1.R == c2.R &&
		c1.G == c2.G &&
		c1.B == c2.B &&
		c1.A == c2.A;
}

static bool operator!=(const Color& c1, const Color& c2)
{
	return ((c1 == c2) == false);
}
}

#endif // COLOR_H