#import <UIKit/UIKit.h>
#import "IOSGame_c.h"
#import "../ViewController.h"

void IOSGame_GetScreenSize(int* width, int* height)
{
	CGRect screenRect = [[UIScreen mainScreen] bounds];
	float scale = [[UIScreen mainScreen] scale];
	*width = screenRect.size.height * scale;
	*height = screenRect.size.width * scale;
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