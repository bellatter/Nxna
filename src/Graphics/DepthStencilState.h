#ifndef GRAPHICS_DEPTHSTENCILSTATE_H
#define GRAPHICS_DEPTHSTENCILSTATE_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(CompareFunction)
		Always,
		Equal,
		Greater,
		GreaterEqual,
		Less,
		LessEqual,
		Never,
		NotEqual
	END_NXNA_ENUM(CompareFunction)

	NXNA_ENUM(StencilOperation)
		Decrement,
		DecrementSaturation,
		Increment,
		IncrementSaturation,
		Invert,
		Keep,
		Replace,
		Zero
	END_NXNA_ENUM(StencilOperation)

	class GraphicsDevice;

	class DepthStencilState
	{
		friend class GraphicsDevice;

		static DepthStencilState m_default;
		static DepthStencilState m_depthRead;
		static DepthStencilState m_none;

		void* m_handle;

	public:

		DepthStencilState()
		{
			DepthBufferFunction = CompareFunction::LessEqual;
			DepthBufferEnable = true;
			DepthBufferWriteEnable = true;

			ReferenceStencil = 0;
			StencilEnable = false;
			StencilFunction = CompareFunction::Always;
			StencilPass = StencilOperation::Keep;

			m_handle = nullptr;
		}

		CompareFunction DepthBufferFunction;
		bool DepthBufferEnable;
		bool DepthBufferWriteEnable;

		int ReferenceStencil;
		bool StencilEnable;
		CompareFunction StencilFunction;
		StencilOperation StencilPass;

		static const DepthStencilState* GetDefault()
		{
			if (m_isInitialized == false)
				init();

			return &m_default;
		}

		static const DepthStencilState* GetDepthRead()
		{
			if (m_isInitialized == false)
				init();

			return &m_depthRead;
		}

		static const DepthStencilState* GetNone()
		{
			if (m_isInitialized == false)
				init();

			return &m_none;
		}

	private:
		static bool m_isInitialized;
		static void init();
	};
}
}

#endif // GRAPHICS_DEPTHSTENCILSTATE_H