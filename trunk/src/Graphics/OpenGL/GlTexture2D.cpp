#include "OpenGL.h"
#include "GlTexture2D.h"
#include "OpenGLDevice.h"
#include "../SamplerState.h"
#include "../GraphicsDeviceCapabilities.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	GlTexture2D::GlTexture2D(GraphicsDevice* device, int width, int height, SurfaceFormat format)
		: Texture2D(device)
	{
		m_width = width;
		m_height = height;
		m_format = format;
		m_hasMipmaps = false;

		glGenTextures(1, &m_glTex);
		glBindTexture(GL_TEXTURE_2D, m_glTex);

		SetSamplerState(&m_samplerState);

		GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void GlTexture2D::SetData(int level, byte* pixels, int length)
	{
		int mipWidth = m_width >> level;
		int mipHeight = m_height >> level;

		glBindTexture(GL_TEXTURE_2D, m_glTex);

		if (m_format == SurfaceFormat::Dxt1 || m_format == SurfaceFormat::Dxt3 || m_format == SurfaceFormat::Dxt5)
		{
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F3
#endif

			if (m_device->GetCaps()->SupportsS3tcTextureCompression == false)
			{
				byte* converted;
				if (m_format == SurfaceFormat::Dxt1)
					converted = DecompressDxtc1(pixels, mipWidth, mipHeight, length);
				else if (m_format == SurfaceFormat::Dxt3)
					converted = DecompressDxtc3(pixels, mipWidth, mipHeight, length);
				else
					converted = DecompressDxtc5(pixels, mipWidth, mipHeight, length);
				glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, mipWidth, mipHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, converted);
				delete[] converted;
			}
			else
			{
				if (m_format == SurfaceFormat::Dxt1)
					glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, mipWidth, mipHeight, 0, length, pixels);
				else if (m_format == SurfaceFormat::Dxt3)
					glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, mipWidth, mipHeight, 0, length, pixels);
				else
					glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, mipWidth, mipHeight, 0, length, pixels);
			}
		}
		else if (m_format == SurfaceFormat::Pvrtc4)
		{
#ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#endif
			glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, mipWidth, mipHeight, 0, length, pixels);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, mipWidth, mipHeight, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		}

		if (level > 0 && m_hasMipmaps == false)
		{
			m_hasMipmaps = true;

			// reapply the sampler state to turn on mipmapping
			SetSamplerState(&m_samplerState);
		}

		GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void GlTexture2D::SetSamplerState(const SamplerState* state)
	{
		glBindTexture(GL_TEXTURE_2D, m_glTex);

		int minFilter, magFilter;
		if (state->Filter == TextureFilter::Linear)
		{
			minFilter = (m_hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			magFilter = GL_LINEAR;
		}
		else if (state->Filter == TextureFilter::Point)
		{
			minFilter = (m_hasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
			magFilter = GL_NEAREST;
		}
		else if (state->Filter == TextureFilter::LinearMipPoint)
		{
			// TODO: what should we do if they want a mipmap filter but the texture isn't mipmapped?
			minFilter = GL_LINEAR_MIPMAP_NEAREST;
			magFilter = GL_LINEAR;
		}
		else if (state->Filter == TextureFilter::PointMipLinear)
		{
			minFilter = GL_NEAREST_MIPMAP_LINEAR;
			magFilter = GL_NEAREST;
		}
		else if (state->Filter == TextureFilter::MinLinearMagPointMipLinear)
		{
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			magFilter = GL_NEAREST;
		}
		else if (state->Filter == TextureFilter::MinLinearMagPointMipPoint)
		{
			minFilter = GL_LINEAR_MIPMAP_NEAREST;
			magFilter = GL_NEAREST;
		}
		else if (state->Filter == TextureFilter::MinPointMagLinearMipLinear)
		{
			minFilter = GL_NEAREST_MIPMAP_LINEAR;
			magFilter = GL_LINEAR;
		}
		else if (state->Filter == TextureFilter::MinPointMagLinearMipPoint)
		{
			minFilter = GL_NEAREST_MIPMAP_NEAREST;
			magFilter = GL_LINEAR;
		}

		int wrapS = convertAddressMode(state->AddressU);
		int wrapT = convertAddressMode(state->AddressV);
		int wrapR = convertAddressMode(state->AddressW);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
#ifndef USING_OPENGLES
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapR);
#endif

		m_samplerState = *state;
	}

	int GlTexture2D::convertAddressMode(TextureAddressMode mode)
	{
		if (mode == TextureAddressMode::Wrap)
			return GL_REPEAT;
		if (mode == TextureAddressMode::Clamp)
			return GL_CLAMP_TO_EDGE;
		
		return GL_MIRRORED_REPEAT;
	}

}
}
}