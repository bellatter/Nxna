#ifndef GRAPHICS_OPENGL_GLVERTEXBUFFER_H
#define GRAPHICS_OPENGL_GLVERTEXBUFFER_H

#include "../VertexBuffer.h"
#include "../VertexDeclaration.h"
#include "../IVertexBufferPimpl.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlVertexBuffer : public Pvt::IVertexBufferPimpl
	{
	protected:
		unsigned int m_buffer;
		VertexDeclaration m_declaration;
		int m_vertexCount;

	public:
		GlVertexBuffer(bool dynamic, OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);

		void Bind() const;

	protected:
		virtual void SetData(int offsetInBytes, void* data, int numBytes) override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_OPENGL_GLVERTEXBUFFER_H