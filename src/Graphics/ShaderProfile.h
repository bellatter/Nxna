#ifndef NXNA_GRAPHICS_SHADERPROFILE_H
#define NXNA_GRAPHICS_SHADERPROFILE_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(ShaderProfile)
		ANY_GLSL = 0x1000,
		ANY_GLSL_ES = ANY_GLSL | 0x0800,
		ANY_HLSL = 0x2000,

		GLSL_120 = ANY_GLSL | 1,
		GLSL_130 = ANY_GLSL | 2,
		GLSL_140 = ANY_GLSL | 3,
		GLSL_150 = ANY_GLSL | 4,
		GLSL_330 = ANY_GLSL | 5,
		GLSL_400 = ANY_GLSL | 6,
		GLSL_410 = ANY_GLSL | 7,
		GLSL_420 = ANY_GLSL | 8,
		GLSL_430 = ANY_GLSL | 9,
		GLSL_440 = ANY_GLSL | 10,

		GLSL_ES_100 = ANY_GLSL_ES | 1,
		GLSL_ES_300 = ANY_GLSL_ES | 2,

		HLSL_2_0 = ANY_HLSL | 1,
		HLSL_2_a = ANY_HLSL | 2,
		HLSL_2_b = ANY_HLSL | 3,
		HLSL_3_0 = ANY_HLSL | 4,
		HLSL_4_0 = ANY_HLSL | 5,
		HLSL_4_1 = ANY_HLSL | 6,
		HLSL_4_0_level_9_0 = ANY_HLSL | 7,
		HLSL_4_0_level_9_1 = ANY_HLSL | 8,
		HLSL_4_0_level_9_3 = ANY_HLSL | 9,
		HLSL_5_0 = ANY_HLSL | 10,

		PROFILE_UNKNOWN = 0xffff
	END_NXNA_ENUM(ShaderProfile)
}
}

#endif // NXNA_GRAPHICS_SHADERPROFILE_H
