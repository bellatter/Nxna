#ifndef MATH_VECTOR4_H
#define MATH_VECTOR4_H

namespace Nxna
{
	struct Matrix;

	class Vector4
	{
	public:

		float X, Y, Z, W;

		Vector4()
		{
			X = Y = Z = W = 0;
		}

		Vector4(float x, float y, float z, float w)
		{
			X = x; 
			Y = y; 
			Z = z;
			W = w;
		}

		Vector4 operator /(float s) const
		{
			return Vector4(X / s, Y / s, Z / s, W / s);
		}

		static void Transform(const Vector4& v, const Matrix& matrix, Vector4& result);
	};
}

#endif // MATH_VECTOR4_H