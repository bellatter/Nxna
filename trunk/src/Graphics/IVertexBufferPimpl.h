#ifndef GRAPHICS_IVERTEXBUFFERPIMPL_H
#define GRAPHICS_IVERTEXBUFFERPIMPL_H

namespace Nxna
{
namespace Graphics
{
	class IVertexBufferPimpl
	{
	public:
		virtual void SetData(void* data) = 0;
	};

	class IDynamicVertexBufferPimpl
	{
	public:
		virtual void SetData(void* data, int numVertices) = 0;
	};
}
}

#endif // GRAPHICS_IVERTEXBUFFERPIMPL_H
