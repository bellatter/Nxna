#ifndef GRAPHICS_BLENDSTATE_H
#define GRAPHICS_BLENDSTATE_H

namespace Nxna
{
namespace Graphics
{
	enum Blend
	{
		Blend_Zero,
		Blend_One,
		Blend_SourceAlpha,
		Blend_InverseSourceAlpha,
		Blend_DestinationAlpha,
		Blend_InverseDestinationAlpha
	};

	enum BlendFunction
	{
		BlendFunction_Add,
		BlendFunction_Max,
		BlendFunction_Min,
		BlendFunction_ReverseSubtract,
		BlendFunction_Subtract
	};

	class GraphicsDevice;

	class BlendState
	{
		friend GraphicsDevice;

		static BlendState m_additive;
		static BlendState m_alphaBlend;
		static BlendState m_nonPreMultiplied;
		static BlendState m_opaque;

		void* m_handle;

	public:

		BlendState()
		{
			AlphaBlendFunction = BlendFunction_Add;
			AlphaDestinationBlend = Blend_One;
			AlphaSourceBlend = Blend_One;

			ColorBlendFunction = BlendFunction_Add;
			ColorDestinationBlend = Blend_One;
			ColorSourceBlend = Blend_One;

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
