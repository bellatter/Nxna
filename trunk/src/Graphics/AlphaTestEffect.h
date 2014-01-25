#ifndef NXNA_GRAPHICS_ALPHATESTEFFECT_H
#define NXNA_GRAPHICS_ALPHATESTEFFECT_H

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
		class AlphaTestEffectPimpl;
	}

	class AlphaTestEffect : public Effect
	{
		Pvt::AlphaTestEffectPimpl* m_atePimpl; 

	public:

		AlphaTestEffect(GraphicsDevice* device);
		virtual ~AlphaTestEffect() {}

		bool IsVertexColorEnabled();
		void IsVertexColorEnabled(bool enabled);

		void SetWorld(const Matrix& matrix);
		void SetView(const Matrix& matrix);
		void SetProjection(const Matrix& matrix);

		void SetTexture(Texture2D* texture);

		void SetReferenceAlpha(float alpha);
		float GetReferenceAlpha();

		void SetAlpha(float alpha);
		float GetAlpha();

		void SetAlphaFunction(CompareFunction function);
		CompareFunction GetAlphaFunction();

	protected:
		virtual void OnApply() override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_ALPHATESTEFFECT_H