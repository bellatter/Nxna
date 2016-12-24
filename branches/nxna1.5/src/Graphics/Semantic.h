#ifndef NXNA_GRAPHICS_SEMANTIC_H
#define NXNA_GRAPHICS_SEMANTIC_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(Semantic)
		Binormal = 1,
		BlendIndices = 2,
		BlendWeights = 3,
		Color = 4,
		Normal = 5,
		Position = 6,
		PointSize = 7,
		Tangent = 8,
		TexCoord = 9,
		Fog = 10,
		TessFactor = 11,
		VFace = 12,
		VPos = 13,
		Depth = 14
	END_NXNA_ENUM(Semantic)
}
}

#endif // NXNA_GRAPHICS_SEMANTIC_H
