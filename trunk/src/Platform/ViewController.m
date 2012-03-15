//
//  ViewController.m
//  test
//
//  Created by Brad on 3/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <GameKit/GameKit.h>
#import "ViewController.h"
#import "GameCenterManager.h"
#import "iOS/IOSGame_c.h"

ViewController* g_instance;

@interface ViewController () {
   
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation ViewController

@synthesize context = _context;
@synthesize effect = _effect;

- (void)dealloc
{
    [_context release];
    [_effect release];
    [super dealloc];
}

extern GameCenterManager* g_gcm;
float touchPointScale = 0;


- (void)viewDidLoad
{
    [super viewDidLoad];
	
	g_instance = self;
    
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
	
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
	//view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
	self.preferredFramesPerSecond = 60;
    
    [self setupGL];
	
	touchPointScale = [[UIScreen mainScreen] scale];
	
	//if (g_gcm == NULL)
	//	g_gcm = [[GameCenterManager alloc] init];
	
	//[g_gcm setDelegate: self];
	//[g_gcm authenticateLocalUser];
	
	IOSGame_Init();
}

- (void)viewDidUnload
{    
    [super viewDidUnload];
    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown &&
				interfaceOrientation != UIInterfaceOrientationPortrait);
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    // setup
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    // shutdown
}

#pragma mark - GLKView and GLKViewController delegate methods

float timeRemainder = 0;
- (void)update
{
	float timeSince = self.timeSinceLastUpdate;
	
	const float stepSize = 1.0f / 60.0f;
	
	int timesToRun = (int)((timeSince + timeRemainder) / stepSize);
	if (timesToRun > 20)
	{
		timesToRun = 20;
		timeRemainder = 0;
	}
	
	for (int i = 0; i < timesToRun; i++)
	{
		// update
		IOSGame_Update(stepSize);
	}
	
	timeRemainder += timeSince - timesToRun * stepSize;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	int mask;
	glGetIntegerv(GL_DEPTH_WRITEMASK, &mask);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDepthMask((GLboolean)mask);	
	
	// draw
	IOSGame_Draw(self.timeSinceLastDraw);
}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	for (UITouch *touch in touches)
	{
		CGPoint p = [touch locationInView: self.view];
		
		IOSGame_TouchDown(p.x * touchPointScale, p.y * touchPointScale);
	}
}

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	for (UITouch *touch in touches)
	{
		CGPoint p = [touch locationInView: self.view];
		
		IOSGame_TouchMoved(p.x * touchPointScale, p.y * touchPointScale);
	}
}

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent *)event
{
	for (UITouch *touch in touches)
	{
		CGPoint p = [touch locationInView: self.view];
		
		IOSGame_TouchUp(p.x * touchPointScale, p.y * touchPointScale);
	}
}

-(void)showLeaderboard: (NSString*)category: (int)scope
{
	GKLeaderboardViewController* leaderboardController = [[GKLeaderboardViewController alloc] init];
	if (leaderboardController != NULL)
	{
		leaderboardController.category = category;
		leaderboardController.timeScope = scope;
		leaderboardController.leaderboardDelegate = self;
		[self presentModalViewController: leaderboardController animated:YES];
	}
}

-(void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController*) viewController
{
	[self dismissModalViewControllerAnimated: YES];
	[viewController release];
}

-(void)showAchievements
{
	GKAchievementViewController* achievements = [[GKAchievementViewController alloc] init];
	if (achievements != NULL)
	{
		achievements.achievementDelegate = self;
		[self presentModalViewController: achievements animated: YES];
	}
}

-(void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController
{
	[self dismissModalViewControllerAnimated: YES];
	[viewController release];
}

+(ViewController*)getInstance
{
	return g_instance;
}

@end
