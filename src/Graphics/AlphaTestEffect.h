#ifndef NXNA_GRAPHICS_ALPHATESTEFFECT_H
#define NXNA_GRAPHICS_ALPHATESTEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

namespace Nxna
{
namespace Graphics
{
	class AlphaTestEffect : public virtual Effect
	{
	public:

		virtual ~AlphaTestEffect() {}

		virtual bool IsVertexColorEnabled() = 0;
		virtual void IsVertexColorEnabled(bool enabled) = 0;

		virtual void SetWorld(const Matrix& matrix) = 0;
		virtual void SetView(const Matrix& matrix) = 0;
		virtual void SetProjection(const Matrix& matrix) = 0;

		virtual void SetTexture(Texture2D* texture) = 0;

		virtual void SetReferenceAlpha(float alpha) = 0;
		virtual float GetReferenceAlpha() = 0;

		virtual void SetAlpha(float alpha) = 0;
		virtual float GetAlpha() = 0;

		virtual void SetAlphaFunction(CompareFunction function) = 0;
		virtual CompareFunction GetAlphaFunction() = 0;
	};
}
}

#endif // NXNA_GRAPHICS_ALPHATESTEFFECT_H