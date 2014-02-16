#include "GraphicsAdapter.h"

#if defined NXNA_PLATFORMENGINE_SDL
#if defined NXNA_PLATFORM_APPLE
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#endif

namespace Nxna
{
namespace Graphics
{
	std::vector<GraphicsAdapter> GraphicsAdapter::m_adapters;

	std::vector<GraphicsAdapter>& GraphicsAdapter::GetAdapters()
	{
		createAdapters();

		return m_adapters;
	}

	GraphicsAdapter* GraphicsAdapter::GetDefault()
	{
		createAdapters();

		if (m_adapters.empty())
			return nullptr;

		return &m_adapters[0];
	}

	bool convertFormat(unsigned int format, SurfaceFormat& result);
	void insertIfUnique(std::vector<DisplayMode>& modes, DisplayMode mode);

	void GraphicsAdapter::createAdapters()
	{
		if (m_adapters.empty())
		{
#if defined NXNA_PLATFORMENGINE_SDL
			int numAdapters = SDL_GetNumVideoDisplays();

			if (numAdapters > 0)
			{
				m_adapters.reserve(numAdapters);

				for (int i = 0; i < numAdapters; i++)
				{
					m_adapters.push_back(GraphicsAdapter());

					m_adapters[i].m_handle = (void*)i;

					strncpy(m_adapters[i].m_name, SDL_GetDisplayName(i), NAME_MAX_LENGTH);
					m_adapters[i].m_name[NAME_MAX_LENGTH - 1] = 0;

					SDL_DisplayMode mode;
					SDL_GetCurrentDisplayMode(i, &mode);

					m_adapters[i].m_displayMode.Width = mode.w;
					m_adapters[i].m_displayMode.Height = mode.h;

					int numDisplayModes = SDL_GetNumDisplayModes(i);
					m_adapters[i].m_displayModes.reserve(numDisplayModes);

					for (int j = 0; j < numDisplayModes; j++)
					{
						SDL_GetDisplayMode(i, j, &mode);

						DisplayMode supportedMode;
						supportedMode.Width = mode.w;
						supportedMode.Height = mode.h;
						
						if (convertFormat(mode.format, supportedMode.Format) == false)
							continue;

						insertIfUnique(m_adapters[i].m_displayModes, supportedMode);
					}
				}
			}
#else
			// TODO: support the other platform engines (which atm means IOS)
			m_adapters.push_back(GraphicsAdapter());
			strncpy(m_adapters[i].m_name, "IOS Screen", NAME_MAX_LENGTH);

			m_adapters[i].m_displayMode.Width = 0;
			m_adapters[i].m_displayMode.Height = 0;

			DisplayMode supportedMode;
			supportedMode.Width = 0;
			supportedMode.Height = 0;
			supportedMode.Format = SurfaceFormat::Color;

			m_adapters[i].m_displayModes.push_back(supportedMode);
#endif
		}
	}

	bool convertFormat(unsigned int format, SurfaceFormat& result)
	{
#if defined NXNA_PLATFORMENGINE_SDL
		if (format == SDL_PIXELFORMAT_ARGB8888 ||
			format == SDL_PIXELFORMAT_ABGR8888 ||
			format == SDL_PIXELFORMAT_BGRA8888 ||
			format == SDL_PIXELFORMAT_RGB888)
		{
			result = SurfaceFormat::Color;
			return true;
		}
		if (format == SDL_PIXELFORMAT_BGR565)
		{
			result = SurfaceFormat::Bgr565;
			return true;
		}
		if (format == SDL_PIXELFORMAT_BGRA5551)
		{
			result = SurfaceFormat::Bgra5551;
			return true;
		}
		if (format == SDL_PIXELFORMAT_BGRA4444)
		{
			result = SurfaceFormat::Bgra4444;
			return true;
		}
#endif

		return false;
	}

	void insertIfUnique(std::vector<DisplayMode>& modes, DisplayMode mode)
	{
		for (unsigned int i = 0; i < modes.size(); i++)
		{
			if (modes[i].Width == mode.Width &&
				modes[i].Height == mode.Height &&
				modes[i].Format == mode.Format)
			{
				return;
			}
		}

		// still here? Must be save to insert.
		modes.push_back(mode);
	}
}
}