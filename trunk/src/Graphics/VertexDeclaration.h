#ifndef GRAPHICS_VERTEXDECLARATION_H
#define GRAPHICS_VERTEXDECLARATION_H

#include <cstring>

namespace Nxna
{
namespace Graphics
{
	enum VertexElementFormat
	{
		VEF_Single = 1,
		VEF_Vector2,
		VEF_Vector3,
		VEF_Vector4,

		VEF_Color
	};

	enum VertexElementUsage
	{
		VEU_Position,
		VEU_Normal,
		VEU_TextureCoordinate,
		VEU_Color
	};

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
			if (elements[numElements - 1].ElementFormat == VEF_Color)
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
