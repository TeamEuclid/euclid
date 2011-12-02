// Copyright (c) 2011 Jess VanDerwalker
//
// Implemetation of the modutil functions
//

#import <stdio.h>
#import "modutil.h"

@implementation ModUtil
-(void) setValueModPower: (int) x andSetMod: (int) n andSetPower (int) p {
  value = x;
  mod = n;
  pwr = p;
}

-(int) getMod {
  rem = value % mod;
  reutrn rem;
}

@end
