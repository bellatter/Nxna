#ifndef MATH_VECTOR2_H
#define MATH_VECTOR2_H

#include <cmath>

namespace Nxna
{
	struct Matrix;

	struct Vector2
	{
		Vector2()
		{
			X = 0; Y = 0;
		}

		Vector2(float v)
		{
			X = v;
			Y = v;
		}

		Vector2(float x, float y)
		{
			this->X = x;
			this->Y = y;
		}

		Vector2 operator-() const
		{
			return Vector2(-X, -Y);
		}

		Vector2 operator *(float s) const
		{
			return Vector2(X * s, Y * s);
		}

		Vector2 operator /(float s) const
		{
			return Vector2(X / s, Y / s);
		}

		Vector2 operator /(const Vector2& v) const
		{
			return Vector2(X / v.X, Y / v.Y);
		}

		Vector2 operator*(const Vector2& v) const
		{
			return Vector2(X * v.X, Y * v.Y);
		}

		Vector2 operator +(const Vector2& v) const
		{
			return Vector2(X + v.X, Y + v.Y);
		}

		Vector2 operator -(const Vector2& v) const
		{
			return Vector2(X - v.X, Y - v.Y);
		}

		Vector2 operator +=(const Vector2& v)
		{
			X += v.X;
			Y += v.Y;

			return *this;
		}

		Vector2 operator -=(const Vector2& v)
		{
			X -= v.X;
			Y -= v.Y;

			return *this;
		}

		Vector2 operator /=(float n)
		{
			X /= n;
			Y /= n;

			return *this;
		}

		Vector2 operator *=(float n)
		{
			X *= n;
			Y *= n;

			return *this;
		}

		Vector2 Rotate(float angle) const
		{
			float sine = sin(angle);
			float cosine = cos(angle);

			Vector2 result;
			result.X = X * cosine - Y * sine;
			result.Y = X * sine + Y * cosine;

			return result;
		}

		float Length() const
		{
			return sqrt(X * X + Y * Y);
		}

		float LengthSquared() const
		{
			return X * X + Y * Y;
		}

		void Normalize();

		float X, Y;

		static void Dot(const Vector2& v1, const Vector2& v2, float& result)
		{
			result = v1.X * v2.X + v1.Y * v2.Y;
		}

		static float Dot(const Vector2& v1, const Vector2& v2)
		{
			return v1.X * v2.X + v1.Y * v2.Y;
		}

		static Vector2 Normalize(const Vector2& v);
		static void Transform(const Vector2& v, const Matrix& m, Vector2& result);
		static float Distance(const Vector2& v1, const Vector2& v2);
		static float DistanceSquared(const Vector2& v1, const Vector2& v2);
		static void DistanceSquared(const Vector2& v1, const Vector2& v2, float& result);

		static const Vector2& One;
		static const Vector2& UnitX;
		static const Vector2& UnitY;

	private:
		static Vector2 m_one;
		static Vector2 m_unitX;
		static Vector2 m_unitY;
	};
}

#endif // MATH_VECTOR2_H
