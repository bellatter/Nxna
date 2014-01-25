#ifndef GRAPHICS_DUALTEXTUREEFFECT_H
#define GRAPHICS_DUALTEXTUREEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

NXNA_DISABLE_OVERRIDE_WARNING

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
	
	protected:

		virtual void OnApply() override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_DUALTEXTUREEFFECT_H