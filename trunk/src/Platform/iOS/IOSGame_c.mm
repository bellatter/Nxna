#import <UIKit/UIKit.h>
#import "IOSGame_c.h"
#import "../ViewController.h"

void IOSGame_GetScreenSize(int* width, int* height)
{
	float scale = [[UIScreen mainScreen] scale];
	CGSize screenSize = [UIScreen mainScreen].bounds.size;
	if ((NSFoundationVersionNumber <= NSFoundationVersionNumber_iOS_7_1) && UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation)) {
		*width = screenSize.height * scale;
		*height = screenSize.width * scale;
	}
	else
	{
		*width = screenSize.width * scale;
		*height = screenSize.height * scale;
	}
}

void IOSGame_ShowLeaderboard(const char* category, int scope)
{
	NSString* str = [[NSString alloc] initWithUTF8String:category];
	
	[[ViewController getInstance] showLeaderboard:str :scope];
	
	[str release];
}

void IOSGame_ShowAchievements()
{
	[[ViewController getInstance] showAchievements];
}