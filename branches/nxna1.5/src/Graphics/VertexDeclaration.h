#ifndef GRAPHICS_VERTEXDECLARATION_H
#define GRAPHICS_VERTEXDECLARATION_H

#include <cstring>
#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(VertexElementFormat)
		Single = 1,
		Vector2,
		Vector3,
		Vector4,

		Color
	END_NXNA_ENUM(VertexElementFormat)

	NXNA_ENUM(VertexElementUsage)
		Position,
		Normal,
		TextureCoordinate,
		Color
	END_NXNA_ENUM(VertexElementUsage)

	struct VertexElement
	{
		int Offset;
		VertexElementFormat ElementFormat;
		VertexElementUsage ElementUsage;
		int UsageIndex;
	};

	class VertexDeclaration
	{
		static const int MAX_ELEMENTS = 16;
		int m_numElements;
		VertexElement m_elements[MAX_ELEMENTS];
		int m_stride;
		unsigned int m_hash;

	public:
		VertexDeclaration(VertexElement* elements, int numElements)
		{
			m_numElements = numElements;
			memcpy(m_elements, elements, sizeof(VertexElement) * numElements);

			int sizeOfFinalElement = 0;
			if (elements[numElements - 1].ElementFormat == VertexElementFormat::Color)
				sizeOfFinalElement = 4;
			else
				sizeOfFinalElement = (int)elements[numElements - 1].ElementFormat * sizeof(float);

			m_stride = elements[numElements - 1].Offset + sizeOfFinalElement;

			m_hash = calcHash();
		}

		int GetStride() const { return m_stride; }
		int GetNumElements() const { return m_numElements; }
		const VertexElement* GetElements() const { return m_elements; }

		unsigned int GetHash() const { return m_hash; }

	private:
		unsigned int calcHash();
	};
}
}

#endif // GRAPHICS_VERTEXDECLARATION_H
