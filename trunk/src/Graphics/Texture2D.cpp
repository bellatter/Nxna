#include <cassert>
#include <cstring>
#include "Texture2D.h"
#include "GraphicsDevice.h"
#include "libsquish/squish.h"
#include "../Content/FileStream.h"
#include "../Content/ContentManager.h"
#include "../Content/XnbReader.h"

namespace Nxna
{
namespace Graphics
{
	void* Texture2DLoader::Read(Content::XnbReader* stream)
	{
		return Texture2D::LoadFrom(stream);
	}

	void Texture2DLoader::Destroy(void* resource)
	{
		delete static_cast<Texture2D*>(resource);
	}

	Texture2D* Texture2D::LoadFrom(Content::XnbReader* stream)
	{
		int typeID = stream->ReadTypeID();
		
		return Texture2D::LoadFrom(stream->GetStream());
	}

	Texture2D* Texture2D::LoadFrom(Content::Stream* stream)
	{
		assert(stream != nullptr);
        
		// Note: MonoGame works a little differently. We expect the format stored in the XNB to
		// be the true format. MonoGame uses only the original XNA format codes, then
		// takes the current platform into account when deciding the true format of the texture.
		// IE: A DXT5 formatted texture on iOS is *actually* a PVRTC4 compressed texture.

        const int FormatColor = 0;
		const int FormatBGR565 = 1;
		const int FormatDXT1 = 4;
		const int FormatDXT3 = 5;
		const int FormatDXT5 = 6;
		const int FormatPVRTC4 = 100; // this is not supported by XNA! this is own own extension!

		int format = stream->ReadInt32();
		if (format != FormatColor && format != FormatBGR565 && format != FormatDXT1 && format != FormatDXT3 && format != FormatDXT5 && format != FormatPVRTC4)
			throw Content::ContentException("Unsupported texture format");

		int width = stream->ReadInt32();
		int height = stream->ReadInt32();
		int mipCount = stream->ReadInt32();
        
        Texture2D* texture;
		if (format == FormatDXT1)
			texture = GraphicsDevice::GetDevice()->CreateTexture(width, height, SurfaceFormat::Dxt1);
		else if (format == FormatDXT3)
			texture = GraphicsDevice::GetDevice()->CreateTexture(width, height, SurfaceFormat::Dxt3);
		else if (format == FormatDXT5)
			texture = GraphicsDevice::GetDevice()->CreateTexture(width, height, SurfaceFormat::Dxt5);
		else if (format == FormatPVRTC4)
			texture = GraphicsDevice::GetDevice()->CreateTexture(width, height, SurfaceFormat::Pvrtc4);
		else
			texture = GraphicsDevice::GetDevice()->CreateTexture(width, height, SurfaceFormat::Color);

		byte* pixels = nullptr;
		int imageSize;
		for (int i = 0; i < mipCount; i++)
		{
			int size = stream->ReadInt32();
			
			pixels = new byte[size];

			stream->Read(pixels, size);
			imageSize = size;

			if (format == 1)
			{
				byte* convertedPixels = convert(pixels, size / 2, format);
				delete[] pixels;
				pixels = convertedPixels;
			}

			texture->SetData(i, pixels, imageSize);

			delete[] pixels;
		}

		return texture;
	}

	byte* Texture2D::convert(byte* pixels, int length, int format)
	{
		if (format == 1)
		{
			byte* result = new byte[length * 4];

			for (int i = 0; i < length; i++)
			{
				unsigned short pixel;
				memcpy(&pixel, &pixels[i * 2], 2);

				byte r, g, b;
				convert565(pixel, &r, &g, &b);

				result[i * 4 + 0] = r;
				result[i * 4 + 1] = g;
				result[i * 4 + 2] = b;
				result[i * 4 + 3] = 255;
			}

			return result;
		}

		return pixels;
	}

	void Texture2D::convert565(unsigned short pixel, byte* r, byte* g, byte* b)
	{
		int tr = ((pixel & 0xF800) >> 11);
        int tg = ((pixel & 0x07E0) >> 5);
        int tb = (pixel & 0x001F);

        // now scale the values up to max of 255
        *r = (byte)(tr * 255 / 31);
        *g = (byte)(tg * 255 / 63);
        *b = (byte)(tb * 255 / 31);
	}

	byte* Texture2D::DecompressDxtc1(const byte* pixels, int width, int height, int size)
	{
		byte* output = new byte[width * height * 4];

		squish::DecompressImage(output, width, height, pixels, squish::kDxt1);

		return output;
	}

	byte* Texture2D::DecompressDxtc3(const byte* pixels, int width, int height, int size)
	{
		byte* output = new byte[width * height * 4];

		squish::DecompressImage(output, width, height, pixels, squish::kDxt3);

		return output;
	}

	byte* Texture2D::DecompressDxtc5(const byte* pixels, int width, int height, int size)
	{
		byte* output = new byte[width * height * 4];

		squish::DecompressImage(output, width, height, pixels, squish::kDxt5);

		return output;
	}
}
}