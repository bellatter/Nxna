#import <UIKit/UIKit.h>

#import "AppDelegate.h"

int NxnaMainIOS(int argc, const char *argv[])
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	
	[pool release];
}

#ifdef __cplusplus
extern "C"
{
#endif
int NxnaMain(int argc, char* argv[]);
	
#ifdef __cplusplus
}
#endif

int main(int argc, char* argv[])
{
	return NxnaMain(argc, argv);
}