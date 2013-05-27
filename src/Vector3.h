#ifndef MATH_VECTOR3_H
#define MATH_VECTOR3_H

#include <cmath>
#include "NxnaConfig.h"

NXNA_DISABLE_NAMELESS_STRUCT_WARNING

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
		{
			X = x;
			Y = y;
			Z = z;
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
			return sqrt(X * X + Y * Y + Z * Z);
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

		static Vector3 GetUp() { return Vector3(0, 1.0f, 0); }
		static Vector3 GetForward() { return Vector3(0, 0, -1.0f); }
		static Vector3 GetZero() { return Vector3(0, 0, 0); }
		static Vector3 GetRight() { return Vector3(1.0f, 0, 0); }

		static void Multiply(const Vector3& v, float scaleFactor, Vector3& result)
		{
			result.X = v.X * scaleFactor;
			result.Y = v.Y * scaleFactor;
			result.Z = v.Z * scaleFactor;
		}

		static void Dot(const Vector3& v1, const Vector3& v2, float& result);
		static void Cross(const Vector3& v1, const Vector3& v2, Vector3& result);
		static Vector3 Normalize(const Vector3& v);
		static void Normalize(const Vector3& v, Vector3& result);
		static Vector3 Transform(const Vector3& v, const Matrix& matrix) { Vector3 result; Transform(v, matrix, result); return result; }
		static void Transform(const Vector3& v, const Matrix& matrix, Vector3& result);
		static void Transform(const Vector3& v, const Nxna::Quaternion& quat, Vector3& result);
		static Vector3 TransformNormal(const Vector3& normal, const Matrix& matrix) { Vector3 v; TransformNormal(normal, matrix, v); return v; }
		static void TransformNormal(const Vector3& normal, const Matrix& matrix, Vector3& result);

		static float DistanceSquared(const Vector3& v1, const Vector3& v2);
	};
}

NXNA_ENABLE_NAMELESS_STRUCT_WARNING

#endif // MATH_VECTOR3_H
