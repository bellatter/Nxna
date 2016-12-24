#include "Vector4.h"
#include "Matrix.h"

namespace Nxna
{
	void Vector4::Transform(const Vector4& v, const Matrix& matrix, Vector4& result)
	{
		float x = v.X * matrix.M11 + v.Y * matrix.M21 + v.Z * matrix.M31 + v.W * matrix.M41;
        float y = v.X * matrix.M12 + v.Y * matrix.M22 + v.Z * matrix.M32 + v.W * matrix.M42;
        float z = v.X * matrix.M13 + v.Y * matrix.M23 + v.Z * matrix.M33 + v.W * matrix.M43;
		float w = v.X * matrix.M14 + v.Y * matrix.M24 + v.Z * matrix.M34 + v.W * matrix.M44;

		result.X = x;
		result.Y = y;
		result.Z = z;
		result.W = w;
	}
}
