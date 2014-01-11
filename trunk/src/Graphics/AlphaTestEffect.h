#ifndef NXNA_GRAPHICS_ALPHATESTEFFECT_H
#define NXNA_GRAPHICS_ALPHATESTEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

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

		virtual void Apply() override;
	};
}
}

#endif // NXNA_GRAPHICS_ALPHATESTEFFECT_H