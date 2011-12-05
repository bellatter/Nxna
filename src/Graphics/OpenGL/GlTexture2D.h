#ifndef GRAPHICS_OPENGL_GLTEXTURE2D_H
#define GRAPHICS_OPENGL_GLTEXTURE2D_H

#include "../Texture2D.h"
#include "../SamplerState.h"

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;

namespace OpenGl
{
	class GlTexture2D : public Texture2D
	{
		unsigned int m_glTex;
		bool m_hasMipmaps;
		SamplerState m_samplerState;

	public:

		GlTexture2D(GraphicsDevice* device, int width, int height, SurfaceFormat format);

		virtual void SetData(int level, byte* pixels, int length) override;

		unsigned int GetGlTexture() { return m_glTex; }

		virtual void SetSamplerState(const SamplerState* state) override;

	private:
		static int convertAddressMode(TextureAddressMode mode);
	};
}
}
}

#endif // GRAPHICS_OPENGL_GLTEXTURE2D_H