#include "MathHelper.h"
#include <cmath>

namespace Nxna
{
	float MathHelper::WrapAngle(float angle)
	{
		const float divisor = Pi * 2.0f;
		angle = angle - (divisor * roundf(angle / divisor));

		if (angle <= -Pi)
			angle += divisor;
		else if (angle > Pi)
			angle -= divisor;

		return angle;
	}
}
