#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <GameKit/GameKit.h>
#import <iAd/iAd.h>
#import "GameCenterManager.h"

@interface ViewController : GLKViewController<GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, GameCenterManagerDelegate, ADBannerViewDelegate>
-(void)showLeaderboard: (NSString*)category: (int)scope;
-(void)showAchievements;
+(ViewController*)getInstance;
-(void)enableBannerAds:(BOOL)enabled;
-(void)hideBannerAds;
@end
