#ifndef GRAPHICS_OPENGL_GLTEXTURE2D_H
#define GRAPHICS_OPENGL_GLTEXTURE2D_H

#include "../Texture2D.h"
#include "../ITexture2DPimpl.h"
#include "../SamplerState.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;

namespace OpenGl
{
	class GlTexture2D : public Pvt::ITexture2DPimpl
	{
		GraphicsDevice* m_device;
		int m_width;
		int m_height;
		unsigned int m_glTex;
		bool m_hasMipmaps;
		SurfaceFormat m_format;
		SamplerState m_samplerState;

	public:

		GlTexture2D(GraphicsDevice* device, int width, int height, SurfaceFormat format);
		virtual ~GlTexture2D();

		virtual void SetData(int level, byte* pixels, int length) override;

		unsigned int GetGlTexture() { return m_glTex; }

		virtual void SetSamplerState(const SamplerState* state);// override;

	private:
		static int convertAddressMode(TextureAddressMode mode);
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_OPENGL_GLTEXTURE2D_H