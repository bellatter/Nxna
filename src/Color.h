#ifndef COLOR_H
#define COLOR_H

#include "NxnaConfig.h"
#include "MathHelper.h"

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
		float r = R * s;
		float g = G * s;
		float b = B * s;
		float a = A * s;

		// clamp
		if (r > 255.0f)
			r = 255.0f;
		else if (r < 0)
			r = 0;

		if (g > 255.0f)
			g = 255.0f;
		else if (g < 0)
			g = 0;

		if (b > 255.0f)
			b = 255.0f;
		else if (b < 0)
			b = 0;

		if (a > 255.0f)
			a = 255.0f;
		else if (a < 0)
			a = 0;

		Color c;
		c.R = (byte)r;
		c.G = (byte)g;
		c.B = (byte)b;
		c.A = (byte)a;

		return c;
	}

	static Color GetWhite() { return Color(255, 255, 255); }
	static Color GetBisque() { return Color(255, 228, 196); }
	static Color GetBlack() { return Color(0, 0, 0); }
	static Color GetBurlyWood() { return Color(222, 184, 135); }
	static Color GetDarkOrange() { return Color(255, 140, 0); }
	static Color GetDeepSkyBlue() { return Color(0, 191, 255); }
	static Color GetDodgerBlue() { return Color(30, 144, 255); }
	static Color GetFuchsia() { return Color(255, 0, 255); }
	static Color GetGray() { return Color(128, 128, 128); }
	static Color GetCornflowerBlue() { return Color(100, 149, 237); }
	static Color GetGold() { return Color(255, 215, 0); }
	static Color GetGoldenrod() { return Color(218, 165, 32); }
	static Color GetIndianRed() { return Color(205, 92, 92); }
	static Color GetRed() { return Color(255, 0, 0); }
	static Color GetSaddleBrown() { return Color(139, 69, 19); }
	static Color GetSandyBrown() { return Color(244, 164, 96); }
	static Color GetTomato() { return Color(255, 99, 71); }

	static Color Lerp(const Color& c1, const Color& c2, float amount)
	{
		return Color((int)MathHelper::Lerp(c1.R, c2.R, amount),
			(int)MathHelper::Lerp(c1.G, c2.G, amount),
			(int)MathHelper::Lerp(c1.B, c2.B, amount),
			(int)MathHelper::Lerp(c1.A, c2.A, amount));
	}

	static Color FromNonPremultiplied(int r, int g, int b, int a)
	{
		return Color((byte)(r * a / 255), (byte)(g * a / 255), (byte)(b * a / 255), (byte)a);
	}

	bool operator==(const Color& c) const
	{
		return R == c.R &&
			G == c.G &&
			B == c.B &&
			A == c.A;
	}

	bool operator!=(const Color& c) const
	{
		return !(*this == c);
	}
};

}

#endif // COLOR_H