#ifndef NXNA_MATHHELPER_H
#define NXNA_MATHHELPER_H

#include <cmath>

namespace Nxna
{
	// This imitates the System.Math class
	class Math
	{
	public:
		static float Round(float n)
		{
			return floor(n + 0.5f);
		}

		static int Roundi(float n)
		{
			return (int)(n + 0.5f);
		}
	};

	class MathHelper
	{
	public:

		static const float E;
		static const float Pi;
		static const float TwoPi;
		static const float PiOver2;
		static const float PiOver4;

		static float Min(float a, float b)
		{
			return (a < b ? a : b);
		}

		static float Max(float a, float b)
		{
			return (a > b ? a : b);
		}

		static float Clamp(float value, float min, float max)
		{
			if (value < min) return min;
			if (value > max) return max;

			return value;
		}

		static float ToRadians(float degrees)
		{
			return degrees * 0.0174532925f;
		}

		static float WrapAngle(float angle)
		{
			const float divisor = Pi * 2.0f;
			angle = angle - (divisor * Math::Round(angle / divisor));

			if (angle <= -Pi)
				angle += divisor;
			else if (angle > Pi)
				angle -= divisor;

			return angle;
		}
	};
}

#endif // NXNA_MATHHELPER_H
