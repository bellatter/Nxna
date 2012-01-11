#ifndef GRAPHICS_TEXTURE2D_H
#define GRAPHICS_TEXTURE2D_H

#include "../NxnaConfig.h"
#include "../Content/ContentManager.h"

namespace Nxna
{
namespace Content
{
	class FileStream;
}

namespace Graphics
{
	enum SurfaceFormat
	{
		SurfaceFormat_Color,
		SurfaceFormat_Dxt1,
		SurfaceFormat_Dxt3,
		SurfaceFormat_Dxt5,

		// the following are not supported by XNA. These are our own
		// extensions so that iOS devices can have compressed textures too.
		SurfaceFormat_Pvrtc4
	};

	class GraphicsDevice;
	class SamplerState;

	class Texture2D 
	{
	protected:
		int m_width;
		int m_height;
		SurfaceFormat m_format;
		GraphicsDevice* m_device;

	public:

		Texture2D(GraphicsDevice* device)
		{
			m_width = 0;
			m_height = 0;
			m_device = device;
		}

		virtual ~Texture2D() { }

		GraphicsDevice* GetGraphicsDevice() { return m_device; }

		int GetWidth() { return m_width; }
		int GetHeight() { return m_height; }

		virtual void SetData(byte* pixels, int length)
		{
			SetData(0, pixels, length);
		}

		virtual void SetData(int level, byte* pixels, int length) = 0;

		// HACK: this is NOT how XNA works! we need to find a better way to do this.
		virtual void SetSamplerState(const SamplerState* state) = 0;

		static Texture2D* LoadFrom(Content::FileStream* stream);
		static Texture2D* LoadFrom(Content::XnbReader* stream);

	protected:
		static byte* DecompressDxtc(const byte* pixels, int width, int height, int size);

	private:
		static byte* convert(byte* pixels, int length, int format);
		static void convert565(unsigned short pixel, byte* r, byte* g, byte* b);
	};

	class Texture2DLoader : public Content::IContentResourceLoader
	{
	public:
		virtual const char* GetTypeName() override { return typeid(Texture2D).name(); }
		virtual void* Load(Content::XnbReader* stream) override;
		virtual void Destroy(void* resource) override;
	};
}
}

#endif // GRAPHICS_TEXTURE2D_H