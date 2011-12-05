#include <cmath>
#include "Quaternion.h"

namespace Nxna
{
	void Quaternion::Multiply(const Quaternion& q1, const Quaternion& q2, Quaternion& result)
	{
		result.X = q1.W * q2.X + q1.X * q2.W + q1.Y * q2.Z - q1.Z * q2.Y;
		result.Y = q1.W * q2.Y + q1.Y * q2.W + q1.Z * q2.X - q1.X * q2.Z;
		result.Z = q1.W * q2.Z + q1.Z * q2.W + q1.X * q2.Y - q1.Y * q2.X;
		result.W = q1.W * q2.W - q1.X * q2.X - q1.Y * q2.Y - q1.Z * q2.Z;
	}

	Quaternion Quaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
	{
		float halfYaw = yaw * 0.5f;
		float halfPitch = pitch * 0.5f;
		float halfRoll = roll * 0.5f;

		float sinYaw = sin(halfYaw);
		float cosYaw = cos(halfYaw);
		float sinPitch = sin(halfPitch);
		float cosPitch = cos(halfPitch);
		float sinRoll = sin(halfRoll);
		float cosRoll = cos(halfRoll);

		Quaternion result;
		result.X = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
		result.Y = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;
		result.Z = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;
		result.W = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;

		return result;
	}
}