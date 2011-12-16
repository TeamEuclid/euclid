// Copyright (c) JessVanDerwalker
//
// Driver for ModUtil
//

#import <stdio.h>
#import "ModUtil.h"

int main(int argc, char **argv) {

  ModUtil *mod = [[ModUtil alloc] init];

  // Set initial values
  [mod setValueModPower: 5 andSetMod: 3 andSetPower: 1];
  
  printf("The modulus is: %d\n", [mod getMod]);

  // Free any memory
  [mod release];

  return 0;
}
