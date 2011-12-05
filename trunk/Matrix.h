#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "Vector3.h"

namespace Nxna
{
	struct Matrix
	{
		Matrix();

		union
		{
			struct
			{
				float M11, M12, M13, M14,
					M21, M22, M23, M24,
					M31, M32, M33, M34,
					M41, M42, M43, M44;
			};
			float C[16];
		};

		static Matrix GetIdentity()
		{
			Matrix r;
			r.M11 = r.M22 = r.M33 = r.M44 = 1.0f;
		
			return r;
		}

		static void GetIdentity(Matrix& m)
		{
			m.M12 = m.M13 = m.M14 =
				m.M21 = m.M23 = m.M24 =
				m.M31 = m.M32 = m.M34 =
				m.M41 = m.M42 = m.M43 = 0;

			m.M11 = m.M22 = m.M33 = m.M44 = 1.0f;
		}

		static Matrix CreateLookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector);
		static Matrix CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);
		static Matrix CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
		static Matrix CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance);
		static void CreateTranslation(float x, float y, float z, Matrix& result);
		static void CreateScale(float x, float y, float z, Matrix& result);
		static void CreateRotationX(float rotation, Matrix& result);
		static void CreateRotationZ(float rotation, Matrix& result);
		static void Invert(const Matrix& matrix, Matrix& result);

		static void Multiply(const Matrix& matrix1, const Matrix& matrix2, Matrix& result);
	};
}

#endif // MATH_MATRIX_H