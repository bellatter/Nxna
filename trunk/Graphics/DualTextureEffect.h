#ifndef GRAPHICS_DUALTEXTUREEFFECT_H
#define GRAPHICS_DUALTEXTUREEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

namespace Nxna
{
namespace Graphics
{
	class DualTextureEffect : public virtual Effect
	{
	public:
		virtual ~DualTextureEffect() {}

		virtual bool IsVertexColorEnabled() = 0;
		virtual void IsVertexColorEnabled(bool enabled) = 0;

		virtual void SetWorld(const Matrix& matrix) = 0;
		virtual void SetView(const Matrix& matrix) = 0;
		virtual void SetProjection(const Matrix& matrix) = 0;

		virtual void SetTexture(Texture2D* texture) = 0;
		virtual void SetTexture2(Texture2D* texture) = 0;
	};
}
}

#endif // GRAPHICS_DUALTEXTUREEFFECT_H