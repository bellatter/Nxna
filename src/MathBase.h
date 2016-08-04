#ifndef NXNA_MATHBASE_H
#define NXNA_MATHBASE_H

#ifdef NXNA_NO_CRT

extern "C"
{
	// rely on intrinsics for all these

	float sinf(float v);
	float cosf(float v);
	float tanf(float v);
	float sqrtf(float v);
	double fabs(float v);

}



#else
#include <cmath>
#endif

//#define USE_SSE2
//#include "sse_mathfun.h"
//
//static float nxna_sinf(float v)
//{
//	v4sf v4 = { v, v, v, v };
//	return sin_ps(v4).m128_f32[0];
//}

#endif // NXNA_MATHBASE_H
