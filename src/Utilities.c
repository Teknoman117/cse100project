//
//  Utilities.c
//  CSE 100 Extra Credit Project 1
//
//  Created by Nathaniel Lewis, Matthew Bucci, Zachary Canann on 4/28/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#include "Utilities.h"
#include <stdlib.h>

// Time utilities for windows
#ifdef WIN32
#include <windows.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;
    
	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);
        
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;
        
		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}
    
	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}
    
	return 0;
}

#endif

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
