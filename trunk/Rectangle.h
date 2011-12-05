#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Vector2.h"

namespace Nxna
{
	struct Rectangle
	{
		int X, Y, Width, Height;

		Rectangle()
		{
			X = Y = Width = Height = 0;
		}

		Rectangle(int x, int y, int width, int height)
		{
			X = x;
			Y = y;
			Width = width;
			Height = height;
		}

		Vector2 GetCenter()
		{
			return Vector2(X + Width * 0.5f, Y + Height * 0.5f);
		}
	};
}

#endif // RECTANGLE_H