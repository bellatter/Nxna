#include "RenderTarget2D.h"
#include "GraphicsDevice.h"
#include "IRenderTarget2DPimpl.h"

namespace Nxna
{
namespace Graphics
{
	RenderTarget2D::RenderTarget2D(GraphicsDevice* device, int width, int height, bool mipMap, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount, RenderTargetUsage usage)
		: Texture2D(device, width, height, mipMap, preferredFormat, true)
	{
		m_pimpl = device->CreateRenderTarget2DPimpl(this, width, height, preferredFormat, preferredDepthFormat, preferredMultiSampleCount, usage);
	}

	RenderTarget2D::~RenderTarget2D()
	{
		delete m_pimpl;
	}
	
	bool RenderTarget2D::FlipBeforeUse()
	{
		return m_pimpl->FlipBeforeUse();
	}
}
}