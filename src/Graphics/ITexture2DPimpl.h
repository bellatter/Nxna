#ifndef NXNA_GRAPHICS_ITEXTURE2DPIMPL_H
#define NXNA_GRAPHICS_ITEXTURE2DPIMPL_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	class ITexture2DPimpl
	{
	public:
		virtual ~ITexture2DPimpl() { }

		virtual void SetData(int level, byte* pixels, int length) = 0;

	protected:
		static byte* DecompressDxtc3(const byte* pixels, int width, int height);
		static byte* DecompressDxtc1(const byte* pixels, int width, int height);
		static byte* DecompressDxtc5(const byte* pixels, int width, int height);
	};
}
}
}

#endif // NXNA_GRAPHICS_ITEXTURE2DPIMPL_H