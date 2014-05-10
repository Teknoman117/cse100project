//
//  Utilities.h
//  CSE 100 Extra Credit Project 1
//
//  Created by Nathaniel Lewis, Matthew Bucci, Zachary Canann on 4/28/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

// Time functions
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

// C only provides a random function capable of generating random 32
// bit integers.  We needed 64 bit precision because of the length
// of generated keys.  This function will generate a pseudorandom
// 64 bit integer
int64_t lrand();
    
#ifdef __cplusplus
}
#endif

// end (ifndef __UTILITIES_H__)
#endif
