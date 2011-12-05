#include <cmath>
#include <cstring>
#include "Matrix.h"

namespace Nxna
{
	Matrix::Matrix()
	{
		memset(C, 0, sizeof(float) * 16);
	}

	Matrix Matrix::CreateLookAt(const Vector3& cameraPosition, const Vector3& cameraTarget, const Vector3& cameraUpVector)
	{
		Vector3 inverseDirection = (cameraPosition - cameraTarget);
		Vector3::Normalize(inverseDirection, inverseDirection);
		
		Vector3 right;
		Vector3 trueUp;

		Vector3::Cross(cameraUpVector, inverseDirection, right);
		Vector3::Cross(inverseDirection, right, trueUp);

		Matrix m;
		m.M11 = right.X;
		m.M12 = trueUp.X;
		m.M13 = inverseDirection.X;
		m.M14 = 0;

		m.M21 = right.Y;
        m.M22 = trueUp.Y;
        m.M23 = inverseDirection.Y;
        m.M24 = 0;

        m.M31 = right.Z;
        m.M32 = trueUp.Z;
        m.M33 = inverseDirection.Z;
        m.M34 = 0;

        m.M41 = -(right.X * cameraPosition.X + right.Y * cameraPosition.Y + right.Z * cameraPosition.Z);
        m.M42 = -(trueUp.X * cameraPosition.X + trueUp.Y * cameraPosition.Y + trueUp.Z * cameraPosition.Z);
        m.M43 = -(inverseDirection.X * cameraPosition.X + inverseDirection.Y * cameraPosition.Y + inverseDirection.Z * cameraPosition.Z);
        m.M44 = 1.0f;

		return m;
	}

	Matrix Matrix::CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
	{
		Matrix result;
		result.M11 = 2.0f / (right - left);
		result.M12 = 0;
		result.M13 = 0;
		result.M14 = 0;
		result.M21 = 0;
		result.M22 = 2.0f / (top - bottom);
		result.M23 = 0;
		result.M24 = 0;
		result.M31 = 0;
		result.M32 = 0;
		result.M33 = 1.0f / (zNearPlane - zFarPlane);
		result.M34 = 0;
		result.M41 = (left + right) / (left - right);
		result.M42 = (top + bottom) / (bottom - top);
		result.M43 = zNearPlane / (zNearPlane - zFarPlane);
		result.M44 = 1.0f;

		return result;
	}

	Matrix Matrix::CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
	{
		float height = 2.0f * (float)tan(fieldOfView * 0.5f) * nearPlaneDistance;

        return CreatePerspective(height * aspectRatio, height, nearPlaneDistance, farPlaneDistance);
	}

	Matrix Matrix::CreatePerspective(float width, float height, float nearPlaneDistance, float farPlaneDistance)
	{
		bool zNegOne = true;

		Matrix m = GetIdentity();

        const float s = -1.0f;
        float invWidth = 1.0f / width;
        float invHeight = 1.0f / height;
        float invDepth = 1.0f / (farPlaneDistance - nearPlaneDistance);
        float near2 = 2.0f * nearPlaneDistance;

        m.M11 = near2 * invWidth;
        m.M22 = near2 * invHeight;
        m.M31 = 0;
        m.M32 = 0;
        m.M34 = s;
        m.M44 = 0;

        if (zNegOne)
        {
            m.M33 = s * (farPlaneDistance + nearPlaneDistance) * invDepth;
            m.M43 = -2.0f * farPlaneDistance * nearPlaneDistance * invDepth;
        }
        else
        {
            m.M33 = s * farPlaneDistance * invDepth;
            m.M43 = -s * nearPlaneDistance * m.M33;
        }
            
        return m;
	 }

	void Matrix::CreateTranslation(float x, float y, float z, Matrix& result)
	{
		GetIdentity(result);

		result.M41 = x;
        result.M42 = y;
        result.M43 = z;
	}

	void Matrix::CreateRotationX(float rotation, Matrix& result)
	{
		float sine = sin(rotation);
        float cosine = cos(rotation);

		GetIdentity(result);
        result.M22 = cosine;
        result.M23 = -sine;
        result.M32 = sine;
        result.M33 = cosine;
	}

	void Matrix::CreateRotationZ(float rotation, Matrix& result)
	{
		float sine = sin(rotation);
        float cosine = cos(rotation);

		GetIdentity(result);
		result.M11 = cosine;
		result.M12 = sine;
		result.M21 = -sine;
		result.M22 = cosine;
	}

	void Matrix::CreateScale(float x, float y, float z, Matrix& result)
	{
		GetIdentity(result);

		result.M11 = x;
        result.M22 = y;
        result.M33 = z;
	}

	void Matrix::Invert(const Matrix& matrix, Matrix& result)
	{
		// this is heavily based on the Mono Xna implementation
        // http://code.google.com/p/monoxna/

        // find determinants
        float det1 = matrix.M11 * matrix.M22 - matrix.M12 * matrix.M21;
        float det2 = matrix.M11 * matrix.M23 - matrix.M13 * matrix.M21;
        float det3 = matrix.M11 * matrix.M24 - matrix.M14 * matrix.M21;
        float det4 = matrix.M12 * matrix.M23 - matrix.M13 * matrix.M22;
        float det5 = matrix.M12 * matrix.M24 - matrix.M14 * matrix.M22;
        float det6 = matrix.M13 * matrix.M24 - matrix.M14 * matrix.M23;
        float det7 = matrix.M31 * matrix.M42 - matrix.M32 * matrix.M41;
        float det8 = matrix.M31 * matrix.M43 - matrix.M33 * matrix.M41;
        float det9 = matrix.M31 * matrix.M44 - matrix.M34 * matrix.M41;
        float det10 = matrix.M32 * matrix.M43 - matrix.M33 * matrix.M42;
        float det11 = matrix.M32 * matrix.M44 - matrix.M34 * matrix.M42;
        float det12 = matrix.M33 * matrix.M44 - matrix.M34 * matrix.M43;
        float major = det1 * det12 - det2 * det11 + det3 * det10 + det4 * det9 - det5 * det8 + det6 * det7;

        float invDetMatrix = 1.0f / major;

        // now do the rest
        result.M11 = (matrix.M22 * det12 - matrix.M23 * det11 + matrix.M24 * det10) * invDetMatrix;
        result.M12 = (-matrix.M12 * det12 + matrix.M13 * det11 - matrix.M14 * det10) * invDetMatrix;
        result.M13 = (matrix.M42 * det6 - matrix.M43 * det5 + matrix.M44 * det4) * invDetMatrix;
        result.M14 = (-matrix.M32 * det6 + matrix.M33 * det5 - matrix.M34 * det4) * invDetMatrix;
        result.M21 = (-matrix.M21 * det12 + matrix.M23 * det9 - matrix.M24 * det8) * invDetMatrix;
        result.M22 = (matrix.M11 * det12 - matrix.M13 * det9 + matrix.M14 * det8) * invDetMatrix;
        result.M23 = (-matrix.M41 * det6 + matrix.M43 * det3 - matrix.M44 * det2) * invDetMatrix;
        result.M24 = (matrix.M31 * det6 - matrix.M33 * det3 + matrix.M34 * det2) * invDetMatrix;
        result.M31 = (matrix.M21 * det11 - matrix.M22 * det9 + matrix.M24 * det7) * invDetMatrix;
        result.M32 = (-matrix.M11 * det11 + matrix.M12 * det9 - matrix.M14 * det7) * invDetMatrix;
        result.M33 = (matrix.M41 * det5 - matrix.M42 * det3 + matrix.M44 * det1) * invDetMatrix;
        result.M34 = (-matrix.M31 * det5 + matrix.M32 * det3 - matrix.M34 * det1) * invDetMatrix;
        result.M41 = (-matrix.M21 * det10 + matrix.M22 * det8 - matrix.M23 * det7) * invDetMatrix;
        result.M42 = (matrix.M11 * det10 - matrix.M12 * det8 + matrix.M13 * det7) * invDetMatrix;
        result.M43 = (-matrix.M41 * det4 + matrix.M42 * det2 - matrix.M43 * det1) * invDetMatrix;
        result.M44 = (matrix.M31 * det4 - matrix.M32 * det2 + matrix.M33 * det1) * invDetMatrix;
	}

	void Matrix::Multiply(const Matrix& matrix1, const Matrix& matrix2, Matrix& result)
	{
		result.M11 = matrix1.M11 * matrix2.M11 + matrix1.M12 * matrix2.M21 + matrix1.M13 * matrix2.M31 + matrix1.M14 * matrix2.M41;
        result.M12 = matrix1.M11 * matrix2.M12 + matrix1.M12 * matrix2.M22 + matrix1.M13 * matrix2.M32 + matrix1.M14 * matrix2.M42;
        result.M13 = matrix1.M11 * matrix2.M13 + matrix1.M12 * matrix2.M23 + matrix1.M13 * matrix2.M33 + matrix1.M14 * matrix2.M43;
        result.M14 = matrix1.M11 * matrix2.M14 + matrix1.M12 * matrix2.M24 + matrix1.M13 * matrix2.M34 + matrix1.M14 * matrix2.M44;
        result.M21 = matrix1.M21 * matrix2.M11 + matrix1.M22 * matrix2.M21 + matrix1.M23 * matrix2.M31 + matrix1.M24 * matrix2.M41;
        result.M22 = matrix1.M21 * matrix2.M12 + matrix1.M22 * matrix2.M22 + matrix1.M23 * matrix2.M32 + matrix1.M24 * matrix2.M42;
        result.M23 = matrix1.M21 * matrix2.M13 + matrix1.M22 * matrix2.M23 + matrix1.M23 * matrix2.M33 + matrix1.M24 * matrix2.M43;
        result.M24 = matrix1.M21 * matrix2.M14 + matrix1.M22 * matrix2.M24 + matrix1.M23 * matrix2.M34 + matrix1.M24 * matrix2.M44;
        result.M31 = matrix1.M31 * matrix2.M11 + matrix1.M32 * matrix2.M21 + matrix1.M33 * matrix2.M31 + matrix1.M34 * matrix2.M41;
        result.M32 = matrix1.M31 * matrix2.M12 + matrix1.M32 * matrix2.M22 + matrix1.M33 * matrix2.M32 + matrix1.M34 * matrix2.M42;
        result.M33 = matrix1.M31 * matrix2.M13 + matrix1.M32 * matrix2.M23 + matrix1.M33 * matrix2.M33 + matrix1.M34 * matrix2.M43;
        result.M34 = matrix1.M31 * matrix2.M14 + matrix1.M32 * matrix2.M24 + matrix1.M33 * matrix2.M34 + matrix1.M34 * matrix2.M44;
        result.M41 = matrix1.M41 * matrix2.M11 + matrix1.M42 * matrix2.M21 + matrix1.M43 * matrix2.M31 + matrix1.M44 * matrix2.M41;
        result.M42 = matrix1.M41 * matrix2.M12 + matrix1.M42 * matrix2.M22 + matrix1.M43 * matrix2.M32 + matrix1.M44 * matrix2.M42;
        result.M43 = matrix1.M41 * matrix2.M13 + matrix1.M42 * matrix2.M23 + matrix1.M43 * matrix2.M33 + matrix1.M44 * matrix2.M43;
        result.M44 = matrix1.M41 * matrix2.M14 + matrix1.M42 * matrix2.M24 + matrix1.M43 * matrix2.M34 + matrix1.M44 * matrix2.M44;
	}
}