#ifndef GRAPHICS_DUALTEXTUREEFFECT_H
#define GRAPHICS_DUALTEXTUREEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

namespace Nxna
{
namespace Graphics
{
	namespace Pvt
	{
		class DualTextureEffectPimpl;
	}

	class DualTextureEffect : public Effect
	{
		Pvt::DualTextureEffectPimpl* m_dtePimpl; 

	public:
		DualTextureEffect(GraphicsDevice* device);
		virtual ~DualTextureEffect() {}

		bool IsVertexColorEnabled();
		void IsVertexColorEnabled(bool enabled);

		void SetWorld(const Matrix& matrix);
		void SetView(const Matrix& matrix);
		void SetProjection(const Matrix& matrix);

		void SetTexture(Texture2D* texture);
		void SetTexture2(Texture2D* texture);

		virtual void Apply() override;
	};
}
}

#endif // GRAPHICS_DUALTEXTUREEFFECT_H