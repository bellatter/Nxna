#ifndef GRAPHICS_DEPTHSTENCILSTATE_H
#define GRAPHICS_DEPTHSTENCILSTATE_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	enum CompareFunction
	{
		CompareFunction_Always,
		CompareFunction_Equal,
		CompareFunction_Greater,
		CompareFunction_GreaterEqual,
		CompareFunction_Less,
		CompareFunction_LessEqual,
		CompareFunction_Never,
		CompareFunction_NotEqual
	};

	enum StencilOperation
	{
		StencilOperation_Decrement,
		StencilOperation_DecrementSaturation,
		StencilOperation_Increment,
		StencilOperation_IncrementSaturation,
		StencilOperation_Invert,
		StencilOperation_Keep,
		StencilOperation_Replace,
		StencilOperation_Zero
	};

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
			DepthBufferFunction = CompareFunction_LessEqual;
			DepthBufferEnable = true;
			DepthBufferWriteEnable = true;

			ReferenceStencil = 0;
			StencilEnable = false;
			StencilFunction = CompareFunction_Always;
			StencilPass = StencilOperation_Keep;

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