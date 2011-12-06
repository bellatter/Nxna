#ifndef GRAPHICS_GLINDEXBUFFER_H
#define GRAPHICS_GLINDEXBUFFER_H

#include "../../NxnaConfig.h"
#include "../IndexBuffer.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlIndexBuffer : public IndexBuffer
	{
		unsigned int m_buffer;

#ifndef NDEBUG
		int* m_indices;
#endif

	public:

		GlIndexBuffer(IndexElementSize elementSize);
		virtual ~GlIndexBuffer();

		virtual void SetData(void* data, int indexCount) override;

		void Bind() const;

#ifndef NDEBUG
		int GetIndex(int index) const;
#endif
	};
}
}
}

#endif // GRAPHICS_GLINDEXBUFFER_H