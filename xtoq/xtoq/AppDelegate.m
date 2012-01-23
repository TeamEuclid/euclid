//
//  AppDelegate.m
//  xtoq
//
//  Created by Jess VanDerwalker on 1/21/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"
#import "AppController.h"

@implementation AppDelegate{
    AppController * ac; 
    
}

@synthesize window = _window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    ac = [[AppController alloc] init];
    [ac showDisplayChooser];    
}

@end
