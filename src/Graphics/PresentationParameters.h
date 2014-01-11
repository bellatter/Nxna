#ifndef NXNA_GRAPHICS_PRESENTATIONPARAMETERS_H
#define NXNA_GRAPHICS_PRESENTATIONPARAMETERS_H

#include "Texture2D.h"
#include "DepthFormat.h"

namespace Nxna
{
namespace Graphics
{
	class PresentationParameters
	{
	public:
		PresentationParameters()
		{
			BackBufferFormat = SurfaceFormat::Color;
			BackBufferWidth = 0;
			BackBufferHeight = 0;
			IsFullScreen = false;
			DepthStencilFormat = DepthFormat::Depth16;
			MultiSampleCount = 0;
		}

		SurfaceFormat BackBufferFormat;
		int BackBufferWidth;
		int BackBufferHeight;
		bool IsFullScreen;

		DepthFormat DepthStencilFormat;
		int MultiSampleCount;
	};
}
}

#endif // NXNA_GRAPHICS_PRESENTATIONPARAMETERS_H
