#ifndef NXNA_GRAPHICS_GRAPHICSDEVICECAPABILITIES_H
#define NXNA_GRAPHICS_GRAPHICSDEVICECAPABILITIES_H

namespace Nxna
{
namespace Graphics
{
	// this class doesn't exist in XNA! Use at your own risk!
	struct GraphicsDeviceCapabilities
	{
		GraphicsDeviceCapabilities()
		{
			SupportsS3tcTextureCompression = false;
			SupportsFullNonPowerOfTwoTextures = false;
			SupportsShaders = false;
		}

		bool SupportsS3tcTextureCompression;
		bool SupportsFullNonPowerOfTwoTextures;
		bool SupportsShaders;
	};
}
}

#endif // NXNA_GRAPHICS_GRAPHICSDEVICECAPABILITIES_H