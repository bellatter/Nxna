#ifndef NXNA_COLOR_H
#define NXNA_COLOR_H

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
			R = (unsigned char)r; G = (unsigned char)g; B = (unsigned char)b; A = 255;
		}

		Color(int r, int g, int b, int a)
		{
			R = (unsigned char)r; G = (unsigned char)g; B = (unsigned char)b; A = (unsigned char)a;
		}

		unsigned char R;
		unsigned char G;
		unsigned char B;
		unsigned char A;
	};

	typedef unsigned int PackedColor;

#define NXNA_GET_PACKED_COLOR(c) (Nxna::PackedColor)((unsigned int)c.A << 24 | (unsigned int)c.B << 16 | (unsigned int)c.G << 8 | c.R)
#define NXNA_GET_PACKED_COLOR_RGB_BYTES(r,g,b) (Nxna::PackedColor)((unsigned int)255 << 24 | (unsigned int)(b & 0xff) << 16 | (unsigned int)(g & 0xff) << 8 | (r & 0xff))
#define NXNA_GET_UNPACKED_COLOR(c, r, g, b, a) { r == c & 0xff; g = (c >> 8) & 0xff; b = (c >> 16) & 0xff; a = (c >> 24) & 0xff; }
}

#endif // NXNA_COLOR_H
