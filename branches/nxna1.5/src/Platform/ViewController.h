#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <GameKit/GameKit.h>
#import "GameCenterManager.h"

@interface ViewController : GLKViewController<GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, GameCenterManagerDelegate>
-(void)showLeaderboard: (NSString*)category: (int)scope;
-(void)showAchievements;
+(ViewController*)getInstance;
@end
