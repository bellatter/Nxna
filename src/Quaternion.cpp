#include "MathBase.h"
#include "Quaternion.h"

namespace Nxna
{
	void Quaternion::Multiply(const Quaternion& q1, const Quaternion& q2, Quaternion& result)
	{
		float x = q1.W * q2.X + q1.X * q2.W + q1.Y * q2.Z - q1.Z * q2.Y;
		float y = q1.W * q2.Y + q1.Y * q2.W + q1.Z * q2.X - q1.X * q2.Z;
		float z = q1.W * q2.Z + q1.Z * q2.W + q1.X * q2.Y - q1.Y * q2.X;
		float w = q1.W * q2.W - q1.X * q2.X - q1.Y * q2.Y - q1.Z * q2.Z;

		result.X = x;
		result.Y = y;
		result.Z = z;
		result.W = w;
	}

	Quaternion Quaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
	{
		float halfYaw = yaw * 0.5f;
		float halfPitch = pitch * 0.5f;
		float halfRoll = roll * 0.5f;

		float sinYaw = sinf(halfYaw);
		float cosYaw = cosf(halfYaw);
		float sinPitch = sinf(halfPitch);
		float cosPitch = cosf(halfPitch);
		float sinRoll = sinf(halfRoll);
		float cosRoll = cosf(halfRoll);

		Quaternion result;
		result.X = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
		result.Y = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;
		result.Z = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;
		result.W = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;

		return result;
	}

	Quaternion Quaternion::CreateFromAxisAngle(float x, float y, float z, float angle)
	{
		float half = angle * 0.5f;
		float sin = sinf(half);
		float cos = cosf(half);
		return Quaternion(x * sin, y * sin, z * sin, cos);
	}

	void Quaternion::Multiply(const Quaternion& q, Nxna::Vector3 v, Nxna::Vector3& result)
	{
		Quaternion q1 = { v.X, v.Y, v.Z, 0 };
		Quaternion conjugate = { -q.X, -q.Y, -q.Z, q.W };
		
		Quaternion q2, q3;
		Multiply(q, q1, q2);
		Multiply(q2, conjugate, q3);

		result = { q3.X, q3.Y, q3.Z };  
	}

	Quaternion Quaternion::Inverse(Quaternion q)
	{
		Quaternion result;
		float ilen = 1.0f / (q.X * q.X + q.Y * q.Y + q.Z * q.Z + q.W * q.W);
		result.X = -q.X * ilen;
		result.Y = -q.Y * ilen;
		result.Z = -q.Z * ilen;
		result.W = q.W * ilen;
		return result;
	}

	void Quaternion::Inverse(const Quaternion& q, Quaternion& result)
	{
		float ilen = 1.0f / (q.X * q.X + q.Y * q.Y + q.Z * q.Z + q.W * q.W);
		result.X = -q.X * ilen;
		result.Y = -q.Y * ilen;
		result.Z = -q.Z * ilen;
		result.W = q.W * ilen;
	}
}
