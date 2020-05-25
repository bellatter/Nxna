#ifndef NXNA_RECTANGLE_H
#define NXNA_RECTANGLE_H

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

#ifdef NXNA_ENABLE_MATH
		Vector2 GetCenter()
		{
			return Vector2(X + Width * 0.5f, Y + Height * 0.5f);
		}
#endif
		void GetCenter(float* centerX, float* centerY)
		{
			if (centerX != nullptr) *centerX = X + Width * 0.5f;
			if (centerY != nullptr) *centerY = Y + Height * 0.5f;
		}

		bool Contains(int x, int y)
		{
			return X <= x && x < X + Width &&
				Y <= y && y < Y + Height;
		}
	};
}

#endif // NXNA_RECTANGLE_H
