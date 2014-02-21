#ifndef NXNA_GRAPHICS_IRENDERTARGET2DPIMPL_H
#define NXNA_GRAPHICS_IRENDERTARGET2DPIMPL_H

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	class IRenderTarget2DPimpl
	{
	public:
		virtual bool FlipBeforeUse() = 0;
	};
}
}
}

#endif // NXNA_GRAPHICS_IRENDERTARGET2DPIMPL_H
