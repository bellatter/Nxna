#include "Viewport.h"

namespace Nxna
{
namespace Graphics
{
#ifdef NXNA_ENABLE_MATH

	Vector3 Viewport::Project(const Vector3& source,
		const Matrix& project,
		const Matrix& view,
		const Matrix& world) const
	{
		Nxna::Matrix transform;
		Nxna::Matrix::Multiply(project, view, transform);
		Nxna::Matrix::Multiply(transform, world, transform);

		return Project(source, transform);
	}

	Vector3 Viewport::Project(const Vector3& source,
		const Matrix& transform) const
	{
		Nxna::Vector4 result;
		result.X = source.X;
		result.Y = source.Y;
		result.Z = source.Z;
		result.W = 1.0f;

		Nxna::Vector4::Transform(result, transform, result);

		// perspective divide
		result = result / result.W;

		Nxna::Vector3 result3;
		result3.X = (result.X + 1.0f) * 0.5f * (float)Width + (float)X;
		result3.Y = (-result.Y + 1.0f) * 0.5f * (float)Height + (float)Y;

		// TODO: i doubt the Z coords are correct since they're not taking into account
		// the clipping plains
		result3.Z = result.Z;

		return result3;
	}
#endif

#ifdef NXNA_ENABLE_MATH
	Nxna::Vector3 Viewport::Unproject(const Nxna::Vector3& source,
		const Nxna::Matrix& projection,
		const Nxna::Matrix& view,
		const Nxna::Matrix& world) const
	{
		Nxna::Vector4 result;
		result.X = ((source.X - X) * 2 / Width) - 1;
		result.Y = 1 - ((source.Y - Y) * 2 / Height);
		result.Z = source.Z;
		result.W = 1.0f;

		Nxna::Matrix invProj, invView, invWorld;
		Nxna::Matrix::Invert(projection, invProj);
		Nxna::Matrix::Invert(view, invView);
		Nxna::Matrix::Invert(world, invWorld);

		Nxna::Vector4::Transform(result, invProj, result);
		Nxna::Vector4::Transform(result, invView, result);
		Nxna::Vector4::Transform(result, invWorld, result);

		result = result / result.W;

		return Nxna::Vector3(result.X, result.Y, result.Z);
    }
#endif
}
}