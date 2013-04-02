#ifndef NXNA_GRAPHICS_VERTEXPOSITIONTEXTURE_H
#define NXNA_GRAPHICS_VERTEXPOSITIONTEXTURE_H

#include <memory>
#include "VertexDeclaration.h"
#include "../Vector2.h"
#include "../Vector3.h"

namespace Nxna
{
namespace Graphics
{
	struct VertexPositionTexture
	{
	private:
		static std::auto_ptr<VertexDeclaration> m_declaration;

	public:
		Vector3 Position;
		Vector2 TextureCoordinate;

		static const VertexDeclaration* GetVertexDeclaration();
	};
}
}

#endif // NXNA_GRAPHICS_VERTEXPOSITIONTEXTURE_H