#ifndef GRAPHICS_BASICEFFECT_H
#define GRAPHICS_BASICEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;
	class Texture2D;

	namespace Pvt
	{
		class BasicEffectPimpl;
	}

	class BasicEffect : public Effect
	{
		Pvt::BasicEffectPimpl* m_bePimpl; 

	public:

		BasicEffect(GraphicsDevice* device);
		virtual ~BasicEffect() {}

		bool IsTextureEnabled();
		void IsTextureEnabled(bool enabled);

		bool IsVertexColorEnabled();
		void IsVertexColorEnabled(bool enabled);

		void SetWorld(const Matrix& matrix);
		void SetView(const Matrix& matrix);
		void SetProjection(const Matrix& matrix);

		void SetTexture(Texture2D* texture);

	protected:

		virtual void OnApply() override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_BASICEFFECT_H