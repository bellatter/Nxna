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

		float X, Y, Z, W;

		static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);

		static void Multiply(const Quaternion& q1, const Quaternion& q2, Quaternion& result);
	};
}

#endif // NXNA_QUATERNION_H