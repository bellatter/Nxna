@protocol GameCenterManagerDelegate <NSObject>
@optional
- (void)processGameCenterAuth:(NSError*)error;
@end
@interface GameCenterManager : NSObject <GameCenterManagerDelegate>
{
	id <GameCenterManagerDelegate, NSObject> delegate;
}
@property(nonatomic, assign) id <GameCenterManagerDelegate, NSObject> delegate;

- (void) authenticateLocalUser;

@end