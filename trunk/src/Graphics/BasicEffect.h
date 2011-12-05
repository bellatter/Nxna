#ifndef GRAPHICS_BASICEFFECT_H
#define GRAPHICS_BASICEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;
	class Texture2D;

	class BasicEffect : public virtual Effect
	{
	public:

		virtual ~BasicEffect() {}

		virtual bool IsTextureEnabled() = 0;
		virtual void IsTextureEnabled(bool enabled) = 0;

		virtual bool IsVertexColorEnabled() = 0;
		virtual void IsVertexColorEnabled(bool enabled) = 0;

		virtual void SetWorld(const Matrix& matrix) = 0;
		virtual void SetView(const Matrix& matrix) = 0;
		virtual void SetProjection(const Matrix& matrix) = 0;

		virtual void SetTexture(Texture2D* texture) = 0;
	};
}
}

#endif // GRAPHICS_BASICEFFECT_H