#include <memory>
#include "VertexPositionTexture.h"


namespace Nxna
{
namespace Graphics
{
	std::auto_ptr<VertexDeclaration> VertexPositionTexture::m_declaration;

	const VertexDeclaration* VertexPositionTexture::GetVertexDeclaration()
	{
		if (m_declaration.get() == nullptr)
		{
			VertexElement elements[] = 
			{
				{0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0},
				{sizeof(float) * 3, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0}
			};

			m_declaration.reset(new VertexDeclaration(elements, 2));
		}

		return m_declaration.get();
	}
}
}