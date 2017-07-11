#ifndef MATH_VECTOR3_H
#define MATH_VECTOR3_H

#include "MathBase.h"

namespace Nxna
{
	struct Quaternion;
	struct Matrix;

	struct Vector3
	{
		Vector3()
		{
			X = 0; Y = 0; Z = 0;
		}

		Vector3(float x, float y, float z)
			: X(x), Y(y), Z(z)
		{
		}

		Vector3 operator *(float s) const
		{
			return Vector3(X * s, Y * s, Z * s);
		}

		Vector3 operator *(const Vector3& v) const
		{
			return Vector3(X * v.X, Y * v.Y, Z * v.Z);
		}

		Vector3 operator /(float s) const
		{
			return Vector3(X / s, Y / s, Z / s);
		}

		Vector3 operator -(const Vector3& v) const
		{
			return Vector3(X - v.X, Y - v.Y, Z - v.Z);
		}

		Vector3 operator-() const
		{
			return Vector3(-X, -Y, -Z);
		}

		Vector3 operator +(const Vector3& v) const
		{
			return Vector3(X + v.X, Y + v.Y, Z + v.Z);
		}

		void operator +=(const Vector3& v)
		{
			X += v.X;
			Y += v.Y;
			Z += v.Z;
		}

		void operator -=(const Vector3& v)
		{
			X -= v.X;
			Y -= v.Y;
			Z -= v.Z;
		}

		void operator *=(float s)
		{
			X *= s;
			Y *= s;
			Z *= s;
		}

		void operator /=(float s)
		{
			X /= s;
			Y /= s;
			Z /= s;
		}

		float Length() const
		{
			return sqrtf(X * X + Y * Y + Z * Z);
		}

		float LengthSquared() const
		{
			return X * X + Y * Y + Z * Z;
		}

		void Normalize();

		union
		{
			struct
			{
				float X, Y, Z;
			};
			float C[3];
		};

		static const Vector3 Up;
		static const Vector3 Forward;
		static const Vector3 Zero;
		static const Vector3 Right;

		static const Vector3 UnitX;
		static const Vector3 UnitY;
		static const Vector3 UnitZ;

		static void Multiply(const Vector3& v, float scaleFactor, Vector3& result)
		{
			result.X = v.X * scaleFactor;
			result.Y = v.Y * scaleFactor;
			result.Z = v.Z * scaleFactor;
		}

		static float Dot(const Vector3& v1, const Vector3& v2);
		static void Dot(const Vector3& v1, const Vector3& v2, float& result);
		static float Dot(const float* v1, const float* v2)
		{
			return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
		}
		
		static Vector3 Cross(const Vector3& v1, const Vector3& v2);
		static void Cross(const Vector3& v1, const Vector3& v2, Vector3& result);
		static void Cross(const float* v1, const float* v2, float* result)
		{
			result[0] = v1[1] * v2[2] - v1[2] * v2[1];
			result[1] = v1[2] * v2[0] - v1[0] * v2[2];
			result[2] = v1[0] * v2[1] - v1[1] * v2[0];
		}
		
		static Vector3 Normalize(const Vector3& v);
		static void Normalize(const Vector3& v, Vector3& result);
		static Vector3 Transform(const Vector3& v, const Matrix& matrix) { Vector3 result; Transform(v, matrix, result); return result; }
		static void Transform(const Vector3& v, const Matrix& matrix, Vector3& result);
		static void Transform(const Vector3& v, const Quaternion& quat, Vector3& result);
		static Vector3 TransformNormal(const Vector3& normal, const Matrix& matrix) { Vector3 v; TransformNormal(normal, matrix, v); return v; }
		static void TransformNormal(const Vector3& normal, const Matrix& matrix, Vector3& result);

		static float Distance(const Vector3& v1, const Vector3& v2);
		static float DistanceSquared(const Vector3& v1, const Vector3& v2);
	};
}

#endif // MATH_VECTOR3_H
