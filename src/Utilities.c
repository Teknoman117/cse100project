//
//  Utilities.c
//  CSE 100 Extra Credit Project 1
//
//  Created by Nathaniel Lewis, Matthew Bucci, Zachary Canann on 4/28/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#include "Utilities.h"
#include <stdlib.h>

// C only provides a random function capable of generating random 32
// bit integers.  We needed 64 bit precision because of the length
// of generated keys.  This function will generate a pseudorandom
// 64 bit integer
int64_t lrand()
{
    // Generate a random upper
    int64_t randomU = rand();
    
	// Generate a random lower
    int64_t randomL = rand();
    
    // Return the mix
    return (randomU << 32) | randomL;
}
