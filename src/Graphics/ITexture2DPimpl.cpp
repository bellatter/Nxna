#include "ITexture2DPimpl.h"
#include "libsquish/squish.h"

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	byte* ITexture2DPimpl::DecompressDxtc1(const byte* pixels, int width, int height)
	{
		byte* output = new byte[width * height * 4];

		squish::DecompressImage(output, width, height, pixels, squish::kDxt1);

		return output;
	}

	byte* ITexture2DPimpl::DecompressDxtc3(const byte* pixels, int width, int height)
	{
		byte* output = new byte[width * height * 4];

		squish::DecompressImage(output, width, height, pixels, squish::kDxt3);

		return output;
	}

	byte* ITexture2DPimpl::DecompressDxtc5(const byte* pixels, int width, int height)
	{
		byte* output = new byte[width * height * 4];

		squish::DecompressImage(output, width, height, pixels, squish::kDxt5);

		return output;
	}
}
}
}

