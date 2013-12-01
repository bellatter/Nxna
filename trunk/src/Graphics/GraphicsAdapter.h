#ifndef NXNA_GRAPHICS_GRAPHICSADAPTER_H
#define NXNA_GRAPHICS_GRAPHICSADAPTER_H

#include <vector>
#include <memory>
#include "Texture2D.h"

namespace Nxna
{
namespace Graphics
{
	struct DisplayMode
	{
		int Width;
		int Height;
		SurfaceFormat Format;
	};

	class GraphicsAdapter
	{
		void* m_handle;
		DisplayMode m_displayMode;
		static const int NAME_MAX_LENGTH = 64;
		char m_name[NAME_MAX_LENGTH];

		std::vector<DisplayMode> m_displayModes;

		static std::vector<GraphicsAdapter> m_adapters;

	public:

		DisplayMode GetCurrentDisplayMode() { return m_displayMode; }

		std::vector<DisplayMode>& GetSupportedDisplayModes() { return m_displayModes; }
		
		static std::vector<GraphicsAdapter>& GetAdapters();
		static GraphicsAdapter* GetDefault();

	private:
		static void createAdapters();
	};
}
}

#endif // NXNA_GRAPHICS_GRAPHICSADAPTER_H
