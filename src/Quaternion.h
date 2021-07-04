#ifndef NXNA_QUATERNION_H
#define NXNA_QUATERNION_H

namespace Nxna
{
	struct Quaternion
	{
	public:

		Quaternion()
		{
			X = Y = Z = W = 0;
		}

		Quaternion(float x, float y, float z, float w)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		Quaternion(float xyzw[])
		{
			X = xyzw[0];
			Y = xyzw[1];
			Z = xyzw[2];
			W = xyzw[3];
		}

		float X, Y, Z, W;

		static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);
		static Quaternion CreateFromAxisAngle(float x, float y, float z, float angle);

		static void Multiply(const Quaternion& q1, const Quaternion& q2, Quaternion& result);
		static void Multiply(const Quaternion& q, Nxna::Vector3 v, Nxna::Vector3& result);

		static Quaternion Inverse(Quaternion q);
		static void Inverse(const Quaternion& q, Quaternion& result);

		static Quaternion Normalize(Quaternion q);
		static void Normalize(const Quaternion& q, Quaternion& result);
	};
}

#endif // NXNA_QUATERNION_H
