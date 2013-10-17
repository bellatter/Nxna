#ifndef GRAPHICS_INDEXBUFFER_H
#define GRAPHICS_INDEXBUFFER_H

#include "VertexBuffer.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(IndexElementSize)
		SixteenBits = 2,
		ThirtyTwoBits = 4
	END_NXNA_ENUM(IndexElementSize)

	class GraphicsDevice;

	namespace Pvt
	{
		class IIndexBufferPimpl;
	}

	class IndexBuffer
	{
		Pvt::IIndexBufferPimpl* m_pimpl;

	protected:
		IndexElementSize m_elementSize;
		int m_indexCount;

	public:

		IndexBuffer(GraphicsDevice* device, IndexElementSize indexElementSize); 
		virtual ~IndexBuffer();

		void SetData(void* indices, int indexCount);

		int GetIndexCount() const { return m_indexCount; }
		IndexElementSize GetElementSize() const { return m_elementSize; }

		Pvt::IIndexBufferPimpl* GetPimpl() { return m_pimpl; }
		const Pvt::IIndexBufferPimpl* GetPimpl() const { return m_pimpl; }
	};
}
}

#endif // GRAPHICS_INDEXBUFFER_H
