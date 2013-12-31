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

	unsigned int GetPackedValue() const
	{
		return (unsigned int)A << 24 | (unsigned int)B << 16 | (unsigned int)G << 8 | R;
	}

	Color operator *(float s) const
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

	static const Color Bisque;
	static const Color Black;
	static const Color Blue;
	static const Color BurlyWood;
	static const Color CornflowerBlue;
	static const Color DarkOrange;
	static const Color DeepSkyBlue;
	static const Color DodgerBlue;
	static const Color Fuchsia;
	static const Color Gray;
	static const Color Gold;
	static const Color Goldenrod;
	static const Color IndianRed;
	static const Color Red;
	static const Color SaddleBrown;
	static const Color SandyBrown;
	static const Color Tomato;
	static const Color White;

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