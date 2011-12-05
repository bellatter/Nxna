#include "DepthStencilState.h"

namespace Nxna
{
namespace Graphics
{
	bool DepthStencilState::m_isInitialized = false;
	DepthStencilState DepthStencilState::m_default;
	DepthStencilState DepthStencilState::m_depthRead;
	DepthStencilState DepthStencilState::m_none;

	void DepthStencilState::init()
	{
		// nothing to change for m_default

		m_depthRead.DepthBufferWriteEnable = false;
		
		m_none.DepthBufferEnable = false;
		m_none.DepthBufferWriteEnable = false;

		m_isInitialized = false;
	}
}
}
