#include "Vector2.h"
#include "Matrix.h"

namespace Nxna
{
	Vector2 Vector2::m_zero;
	Vector2 Vector2::m_one(1.0f, 1.0f);
	Vector2 Vector2::m_unitX(1.0f, 0);
	Vector2 Vector2::m_unitY(0, 1.0f);

	const Vector2& Vector2::Zero(Vector2::m_zero);
	const Vector2& Vector2::One(Vector2::m_one);
	const Vector2& Vector2::UnitX(Vector2::m_unitX);
	const Vector2& Vector2::UnitY(Vector2::m_unitY);

	Vector2 Vector2::Normalize(const Vector2& v)
	{
		float len = v.Length();

		Vector2 r;
		r.X = v.X / len;
		r.Y = v.Y / len;

		return r;
	}

	void Vector2::Normalize()
	{
		float len = Length();

		X /= len;
		Y /= len;
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