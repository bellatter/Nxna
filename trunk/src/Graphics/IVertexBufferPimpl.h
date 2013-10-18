#ifndef GRAPHICS_IVERTEXBUFFERPIMPL_H
#define GRAPHICS_IVERTEXBUFFERPIMPL_H

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	class IVertexBufferPimpl
	{
	protected:
		bool m_dynamic;

	public:
		virtual void SetData(int offsetInBytes, void* data, int numBytes) = 0;
	};
}
}
}

#endif // GRAPHICS_IVERTEXBUFFERPIMPL_H
