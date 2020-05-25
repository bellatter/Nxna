#ifndef NXNA_COLOR_H
#define NXNA_COLOR_H

namespace Nxna
{
	struct Color
	{
#ifndef NXNA_COLOR_API_V2
		Color()
		{
			R = G = B = A = 0;
		}

		Color(int r, int g, int b)
		{
			R = (unsigned char)r; G = (unsigned char)g; B = (unsigned char)b; A = 255;
		}

		Color(int r, int g, int b, int a)
		{
			R = (unsigned char)r; G = (unsigned char)g; B = (unsigned char)b; A = (unsigned char)a;
		}
#endif

		Color operator *(float s) const
		{
			return Color{ (unsigned char)(R * s), (unsigned char)(G * s), (unsigned char)(B * s), (unsigned char)(A * s) };
		}

		static Color Lerp(Color a, Color b, float amount)
		{
			Color r;
			r.R = (unsigned char)MathHelper::Clamp(MathHelper::Lerp(a.R, b.R, amount), 0, 255.0f);
			r.G = (unsigned char)MathHelper::Clamp(MathHelper::Lerp(a.G, b.G, amount), 0, 255.0f);
			r.B = (unsigned char)MathHelper::Clamp(MathHelper::Lerp(a.B, b.B, amount), 0, 255.0f);
			r.A = (unsigned char)MathHelper::Clamp(MathHelper::Lerp(a.A, b.A, amount), 0, 255.0f);

			return r;
		}

#ifdef NXNA_COLOR_API_V2
		static constexpr Color Black()            { return {   0,   0,   0, 255 }; }
		static constexpr Color Blue()             { return {   0,   0, 255, 255 }; }
		static constexpr Color CornflowerBlue()   { return { 100, 149, 237, 255 }; }
		static constexpr Color DeepSkyBlue()      { return {   0, 191, 255, 255 }; }
		static constexpr Color DodgerBlue()       { return {  30, 144, 255, 255 }; }
		static constexpr Color Gold()             { return { 255, 215,   0, 255 }; }
		static constexpr Color Goldenrod()        { return { 218, 165,  32, 255 }; }
		static constexpr Color Gray()             { return { 128, 128, 128, 255 }; }
		static constexpr Color Red()              { return { 255,   0,   0, 255 }; }
		static constexpr Color Tomato()           { return { 255,  99,  71, 255 }; }
		static constexpr Color White()            { return { 255, 255, 255, 255 }; }
		static constexpr Color Yellow()           { return { 255, 255,   0, 255 }; }
#endif

		unsigned char R;
		unsigned char G;
		unsigned char B;
		unsigned char A;
	};

	typedef unsigned int PackedColor;

#define NXNA_GET_PACKED_COLOR(c) (Nxna::PackedColor)((unsigned int)c.A << 24 | (unsigned int)c.B << 16 | (unsigned int)c.G << 8 | c.R)
#define NXNA_GET_PACKED_COLOR_RGB_BYTES(r,g,b) (Nxna::PackedColor)((unsigned int)255 << 24 | (unsigned int)(b & 0xff) << 16 | (unsigned int)(g & 0xff) << 8 | (r & 0xff))
#define NXNA_GET_PACKED_COLOR_RGBA_BYTES(r,g,b,a) (Nxna::PackedColor)((unsigned int)a << 24 | (unsigned int)(b & 0xff) << 16 | (unsigned int)(g & 0xff) << 8 | (r & 0xff))
#define NXNA_GET_UNPACKED_COLOR(c, r, g, b, a) { r == c & 0xff; g = (c >> 8) & 0xff; b = (c >> 16) & 0xff; a = (c >> 24) & 0xff; }

#ifndef NXNA_COLOR_API_V2
#define NXNA_COLOR_BLACK          (Nxna::Color {   0,   0,   0, 255 })
#define NXNA_COLOR_CORNFLOWERBLUE (Nxna::Color { 100, 149, 237, 255 })
#define NXNA_COLOR_DEEPSKYBLUE    (Nxna::Color {   0, 191, 255, 255 })
#define NXNA_COLOR_GOLD           (Nxna::Color { 255, 215,   0, 255 })
#define NXNA_COLOR_GOLDENROD      (Nxna::Color { 218, 165,  32, 255 })
#define NXNA_COLOR_GRAY           (Nxna::Color { 128, 128, 128, 255 })
#define NXNA_COLOR_TOMATO         (Nxna::Color { 255,  99,  71, 255 })
#define NXNA_COLOR_WHITE          (Nxna::Color { 255, 255, 255, 255 })
#endif

}

#endif // NXNA_COLOR_H
