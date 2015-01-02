#ifndef NXNA_GRAPHICS_OPENGL_GLRENDERTARGET2D_H
#define NXNA_GRAPHICS_OPENGL_GLRENDERTARGET2D_H

#include "../RenderTarget2D.h"
#include "../IRenderTarget2DPimpl.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class OpenGlDevice;
	class GlTexture2D;

	class GlRenderTarget2D : public Pvt::IRenderTarget2DPimpl
	{
		unsigned int m_fbo;
		unsigned int m_depthBuffer;
		unsigned int m_stencilBuffer;

	public:

		GlRenderTarget2D(OpenGlDevice* device, GlTexture2D* texture, int width, int height, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount, RenderTargetUsage usage);
		virtual ~GlRenderTarget2D() {}

		unsigned int GetFBO() { return m_fbo; }

		virtual bool FlipBeforeUse() override { return true; }
	};
}
}
}

#endif // NXNA_GRAPHICS_OPENGL_GLRENDERTARGET2D_H
