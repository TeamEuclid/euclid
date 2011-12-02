// Copyright (c) 2011 Jess VanDerwalker
//
// Interface for utility functions for calulating the modulus in
// various circumstances.
//

#import <Foundation/NSObject.h>

@interface ModUtil: NSObject {
  int value;                  // The value on the lhs of mod operation
  int mod;                    // The modulus to use
  int pwr;                    // The power x will be raised to
  int rem;                    // The rhs of the mod operation
}

-(void) setValueModPower: (int) x andSetMod: (int) n andSetPower: (int) p;
-(int) getMod;

@end
