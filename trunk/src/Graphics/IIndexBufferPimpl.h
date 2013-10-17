#ifndef NXNA_GRAPHICS_IINDEXBUFFERPIMPL_H
#define NXNA_GRAPHICS_IINDEXBUFFERPIMPL_H

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	class IIndexBufferPimpl
	{
	protected:
		IndexElementSize m_elementSize;
		int m_indexCount;

	public:
		IIndexBufferPimpl(IndexElementSize size)
		{
			m_elementSize = size;
			m_indexCount = 0;
		}

		virtual ~IIndexBufferPimpl() { }

		virtual void SetData(void* indices, int indexCount) = 0;

		IndexElementSize GetElementSize() const { return m_elementSize; }
		int GetIndexCount() const { return m_indexCount; }
	};
}
}
}

#endif // NXNA_GRAPHICS_IINDEXBUFFERPIMPL_H
