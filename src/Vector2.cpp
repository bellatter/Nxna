#include "Vector2.h"
#include "Matrix.h"

namespace Nxna
{
	Vector2 Vector2::UnitX(1.0f, 0);
	Vector2 Vector2::UnitY(0, 1.0f);

	Vector2 Vector2::Normalize(const Vector2& v)
	{
		float len = v.Length();

		Vector2 r;
		r.X = v.X / len;
		r.Y = v.Y / len;

		return r;
	}

	void Vector2::Transform(const Vector2& v, const Matrix& m, Vector2& result)
	{
		// put the results into temp variables in case v and result are the same
		float x = v.X * m.M11 + v.Y * m.M21 + m.M41;
		float y = v.X * m.M12 + v.Y * m.M22 + m.M42;

		result.X = x;
		result.Y = y;
	}

	float Vector2::Distance(const Vector2& v1, const Vector2& v2)
	{
		return sqrt((v1.X - v2.X) * (v1.X - v2.X) + (v1.Y - v2.Y) * (v1.Y - v2.Y));
	}

	float Vector2::DistanceSquared(const Vector2& v1, const Vector2& v2)
	{
		return (v1.X - v2.X) * (v1.X - v2.X) + (v1.Y - v2.Y) * (v1.Y - v2.Y);
	}

	void Vector2::DistanceSquared(const Vector2& v1, const Vector2& v2, float& result)
	{
		result = (v1.X - v2.X) * (v1.X - v2.X) + (v1.Y - v2.Y) * (v1.Y - v2.Y);
	}
}