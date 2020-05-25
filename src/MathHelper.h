#ifndef NXNA_MATHHELPER_H
#define NXNA_MATHHELPER_H

namespace Nxna
{
class MathHelper
{
public:

	static constexpr float Pi = 3.14159265358979323846f;
	static constexpr float TwoPi = 6.28318530718f;
	static constexpr float PiOver2 = 1.57079632679489661923f;
	static constexpr float PiOver4 = 0.78539816339744830961566084581988f;
	static constexpr float E = 2.71828182845904523536f;

	static inline float Clamp(float value, float min, float max)
	{
		return value < min ? min : (value > max ? max : value);
	}

	static inline int Clampi(int value, int min, int max)
	{
		return value < min ? min : (value > max ? max : value);
	}

	static inline float Lerp(float value1, float value2, float amount)
	{
		return (1.0f - amount) * value1 + amount * value2;
	}

	static float WrapAngle(float angle);

	static constexpr inline float ToRadians(float angle)
	{
		return angle * 0.0174533f;
	}

	static constexpr inline float Min(float a, float b)
	{
		return (a < b) ? a : b;
	}

	static constexpr inline float Max(float a, float b)
	{
		return (a > b) ? a : b;
	}
};
}

#endif // NXNA_MATHHELPER_H