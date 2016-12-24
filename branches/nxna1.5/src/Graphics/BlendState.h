#ifndef GRAPHICS_BLENDSTATE_H
#define GRAPHICS_BLENDSTATE_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(Blend)
		Zero,
		One,
		SourceAlpha,
		InverseSourceAlpha,
		DestinationAlpha,
		InverseDestinationAlpha
	END_NXNA_ENUM(Blend)

	NXNA_ENUM(BlendFunction)
		Add,
		Max,
		Min,
		ReverseSubtract,
		Subtract
	END_NXNA_ENUM(BlendFunction)

	class GraphicsDevice;

	class BlendState
	{
		friend class GraphicsDevice;

		static BlendState m_additive;
		static BlendState m_alphaBlend;
		static BlendState m_nonPreMultiplied;
		static BlendState m_opaque;

		void* m_handle;

	public:

		BlendState()
		{
			AlphaBlendFunction = BlendFunction::Add;
			AlphaDestinationBlend = Blend::One;
			AlphaSourceBlend = Blend::One;

			ColorBlendFunction = BlendFunction::Add;
			ColorDestinationBlend = Blend::One;
			ColorSourceBlend = Blend::One;

			m_handle = nullptr;
		}

		BlendFunction AlphaBlendFunction;
		Blend AlphaDestinationBlend;
		Blend AlphaSourceBlend;
		
		BlendFunction ColorBlendFunction;
		Blend ColorDestinationBlend;
		Blend ColorSourceBlend;

		static const BlendState* GetAdditive()
		{
			if (m_isInitialized == false)
				init();

			return &m_additive;
		}

		static const BlendState* GetAlphaBlend()
		{
			if (m_isInitialized == false)
				init();

			return &m_alphaBlend;
		}

		static const BlendState* GetNonPremultiplied()
		{
			if (m_isInitialized == false)
				init();

			return &m_nonPreMultiplied;
		}

		static const BlendState* GetOpaque()
		{
			if (m_isInitialized == false)
				init();

			return &m_opaque;
		}

	private:
		static bool m_isInitialized;
		static void init();
	};
}
}

#endif // GRAPHICS_BLENDSTATE_H
