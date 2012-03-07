#import <UIKit/UIKit.h>
#import "IOSGame_c.h"

void IOSGame_GetScreenSize(int* width, int* height)
{
	CGRect screenRect = [[UIScreen mainScreen] bounds];
	float scale = [[UIScreen mainScreen] scale];
	*width = screenRect.size.height * scale;
	*height = screenRect.size.width * scale;
}