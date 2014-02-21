#ifndef NXNA_GRAPHICS_RENDERTARGET2D_H
#define NXNA_GRAPHICS_RENDERTARGET2D_H

#include "Texture2D.h"
#include "DepthFormat.h"

namespace Nxna
{
namespace Graphics
{
	namespace Pvt
	{
		class IRenderTarget2DPimpl;
	}

	NXNA_ENUM(RenderTargetUsage)
		DiscardContents = 0,
        PreserveContents = 1,
        PlatformContents = 2
	END_NXNA_ENUM(RenderTargetUsage)

	class RenderTarget2D : public Texture2D
	{
		Pvt::IRenderTarget2DPimpl* m_pimpl;

	public:
		RenderTarget2D(GraphicsDevice* device, int width, int height, bool mipMap, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount, RenderTargetUsage usage);
		virtual ~RenderTarget2D();

		// this is a temporary hack. OpenGL stores textures upside down, so they must be flipped before use. D3D textures don't.
		// eventually we should do something like MonoGame and just render upside down when using OpenGL and RTT.
		bool FlipBeforeUse();

		Pvt::IRenderTarget2DPimpl* GetPimpl() { return m_pimpl; }
	};
}
}

#endif // NXNA_GRAPHICS_RENDERTARGET2D_H
