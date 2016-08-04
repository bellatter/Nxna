#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "Vector3.h"

namespace Nxna
{
	struct Matrix
	{
		Matrix();
		Matrix(float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44);

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

		Matrix operator*(const Matrix& matrix)
		{
			Matrix result;
			Multiply(*this, matrix, result);
			return result;
		}

		static const Matrix Identity;

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
		static Matrix CreateTranslation(float x, float y, float z) { Matrix m; CreateTranslation(x, y, z, m); return m; }
		static void CreateTranslation(float x, float y, float z, Matrix& result);
		static Matrix CreateTranslation(const Vector3& position) { Matrix m; CreateTranslation(position.X, position.Y, position.Z, m); return m; }
		static Matrix CreateScale(float scale) { Matrix m; CreateScale(scale, scale, scale, m); return m; }
		static void CreateScale(float x, float y, float z, Matrix& result);
		static Matrix CreateRotationX(float rotation) { Matrix m; CreateRotationX(rotation, m); return m; }
		static void CreateRotationX(float rotation, Matrix& result);
		static Matrix CreateRotationY(float rotation) { Matrix m; CreateRotationY(rotation, m); return m; }
		static void CreateRotationY(float rotation, Matrix& result);
		static Matrix CreateRotationZ(float rotation) { Matrix m; CreateRotationZ(rotation, m); return m; }
		static void CreateRotationZ(float rotation, Matrix& result);
		static Matrix CreateFromAxisAngle(const Vector3& axis, float angle) { Matrix m; CreateFromAxisAngle(axis, angle, m); return m; }
		static void CreateFromAxisAngle(const Vector3& axis, float angle, Matrix& result);
		static Matrix CreateConstrainedBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& rotationAxis, const Vector3* cameraForwardVector, const Vector3* objectForwardVector) { Matrix m; CreateConstrainedBillboard(objectPosition, cameraPosition, rotationAxis, cameraForwardVector, objectForwardVector, m); return m; }
		static void CreateConstrainedBillboard(const Vector3& objectPosition, const Vector3& cameraPosition, const Vector3& rotationAxis, const Vector3* cameraForwardVector, const Vector3* objectForwardVector, Matrix& result);
		static Matrix CreateWorld(const Vector3& position, const Vector3& forward, const Vector3& up);
		static void Invert(const Matrix& matrix, Matrix& result);

		static void Multiply(const Matrix& matrix1, const Matrix& matrix2, Matrix& result);
	};
}

#endif // MATH_MATRIX_H