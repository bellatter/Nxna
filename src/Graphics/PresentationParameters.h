#ifndef NXNA_GRAPHICS_PRESENTATIONPARAMETERS_H
#define NXNA_GRAPHICS_PRESENTATIONPARAMETERS_H

#include "Texture2D.h"
#include "DepthFormat.h"

namespace Nxna
{
namespace Graphics
{
	enum class WindowMode
	{
		Windowed,
		FullscreenDontCare,

		BorderlessWindowed,
		BorderlessFullscreen,
		ExclusiveFullscreen,
	};

	class PresentationParameters
	{
	public:
		PresentationParameters()
		{
			BackBufferFormat = SurfaceFormat::Color;
			BackBufferWidth = 0;
			BackBufferHeight = 0;
			GameWindowMode = WindowMode::Windowed;
			DepthStencilFormat = DepthFormat::Depth16;
			MultiSampleCount = 0;
		}

		SurfaceFormat BackBufferFormat;
		int BackBufferWidth;
		int BackBufferHeight;
		WindowMode GameWindowMode;

		DepthFormat DepthStencilFormat;
		int MultiSampleCount;
	};
}
}

#endif // NXNA_GRAPHICS_PRESENTATIONPARAMETERS_H
