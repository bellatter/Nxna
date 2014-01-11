#ifndef NXNA_GRAPHICS_DEPTHFORMAT_H
#define NXNA_GRAPHICS_DEPTHFORMAT_H

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(DepthFormat)
		None = 0,
		Depth16 = 1,
		Depth24 = 2,
		Depth24Stencil8 = 3
	END_NXNA_ENUM(DepthFormat)
}
}

#endif // NXNA_GRAPHICS_DEPTHFORMAT_H
