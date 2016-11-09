#ifndef NXNA_PLATFORM_IOS_IOSOPENGLWINDOW_H
#define NXNA_PLATFORM_IOS_IOSOPENGLWINDOW_H

#include "../../NxnaConfig.h"
#include "../../IGraphicsDeviceManager.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Platform
{
namespace iOS
{
	NXNA_ENUM(LeaderboardTimeScope)
		Today,
		Week,
		AllTime
	END_NXNA_ENUM(LeaderboardTimeScope)
	
	class IOSOpenGlWindow : public Nxna::GraphicsDeviceManager
	{
	public:
		IOSOpenGlWindow(Nxna::Game* game);
		virtual Nxna::Graphics::GraphicsDevice* CreateDevice() override;
		virtual void BeginDraw() override;
		virtual void EndDraw() override;
		virtual void ApplyChanges() override;
		virtual void ShowWindow() override;
		virtual void DestroyWindow() override;

		virtual void SetScreenSize(const Nxna::Graphics::PresentationParameters& pp) override;
		
		void ShowLeaderboard(const char* category, LeaderboardTimeScope scope);
		void ShowAchievements();
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_PLATFORM_IOS_IOSOPENGLWINDOW_H
