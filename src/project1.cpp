//
//  project1.cpp
//  CSE 100 Extra Credit Project 1
//
//  Created by Nathaniel Lewis, Matthew Bucci, Zachary Canann on 4/28/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <set>
#include <cstdint>

#include <time.h>
#include <cassert>

#include "HashTable.hpp"
#include "HashFunctions.hpp"

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
#else
#include <sys/time.h>
#endif

// Hash table type
typedef HashTable<int64_t, hash_double<int64_t> > MyHashTable;

// Generate large random integers
int64_t lrand()
{
    // Generate a random upper
    int64_t randomU = rand();
    int64_t randomL = rand();
    
    // Return the mix
    return (randomU << 32) | randomL;
}

// Generate the keys for the experiment
void GenerateKeys(size_t m, size_t n, std::set<int64_t>& S, std::set<int64_t>& D, std::set<int64_t>& U)
{
    // Allocate required memory
    S.clear();
    D.clear();
    U.clear();
    
    // Generate the in hash table set
    while(S.size() < (n / 2))
    {
        // Generate a key
        int64_t key = lrand() % (m * 10000);
        S.insert(key);
    }
    
    // Generate the to delete set
    while(D.size() < (n / 2))
    {
        // Generate a key
        int64_t key = lrand() % (m * 10000);
        
        // if this key does not exist in either array
        if(S.find(key) == S.end())
        {
            D.insert(key);
        }
    }
    
    // Generate the not in table set
    while(U.size() < (n / 2))
    {
        // Generate a key
        int64_t key = (lrand() % (m * 10000)) + (m * 10000);
        U.insert(key);
    }
}

// Run the experiment
void RunExperiment(size_t m, size_t n)
{
    // Datasets for the experiment
    std::set<int64_t> S;
    std::set<int64_t> D;
    std::set<int64_t> U;
    struct timeval start,end;
    
    
    // Make shit
    GenerateKeys(m, n, S, D, U);
    
    
    // Create a hash table w/ open indexing for this lab
    MyHashTable t(m);
    
    
    // Insert the elements from the S set
    for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
    {
        assert(t.insert(*it));
    }
    
    
    // Insert the elements from the D set
    for(std::set<int64_t>::iterator it = D.begin(); it != D.end(); it++)
    {
        assert(t.insert(*it));
    }
    
    
    // Perform the initial experiment
    // Search for the elements in the S set
    gettimeofday(&start,NULL);
    for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
    {
        // Perform the search
        MyHashTable::search_result r = t.search(*it);
        assert(*r == *it);
    }
    gettimeofday(&end,NULL);
    double tS = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000.0;
    tS /= (double) S.size();
    
    
    // Search for the elements in the U set
    gettimeofday(&start,NULL);
    for(std::set<int64_t>::iterator it = U.begin(); it != U.end(); it++)
    {
        // Perform the search
        MyHashTable::search_result r = t.search(*it);
        assert(r == t.NotFound());
    }
    gettimeofday(&end,NULL);
    double tU = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000.0;
    tU /= (double) U.size();
    
    
    // Results
    std::cout << "tS = " << tS << ", tU = " << tU << std::endl;
    std::cout << "tU / tS = " << tU / tS << std::endl;
    
    
    // Remove the elements from the D set
    for(std::set<int64_t>::iterator it = D.begin(); it != D.end(); it++)
    {
        t.remove(*it);
    }
    
    
    // Perform the secondary experiment
    // Search for the elements in the S set
    gettimeofday(&start,NULL);
    for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
    {
        // Perform the search
        MyHashTable::search_result r = t.search(*it);
        assert(*r == *it);
    }
    gettimeofday(&end,NULL);
    double ptS = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000.0;
    ptS /= (double) S.size();
    
    
    // Search for the elements in the U set
    gettimeofday(&start,NULL);
    for(std::set<int64_t>::iterator it = U.begin(); it != U.end(); it++)
    {
        // Perform the search
        MyHashTable::search_result r = t.search(*it);
        assert(r == t.NotFound());
    }
    gettimeofday(&end,NULL);
    double ptU = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000.0;
    ptU /= (double) U.size();
    
    
    // Results
    std::cout << "tS' = " << ptS << ", tU' = " << ptU << std::endl;
    std::cout << "tU' / tS' = " << ptU / ptS << std::endl;
}

// Main method
int main(int argc, const char * argv[])
{
    int tablesize;
    int num_of_elements;
    int testnumber = 0;
    
    do
    {
        do
        {
            //output the test number
            std::cout << "TEST " << testnumber << std::endl;
            
            //get the table size
            std::cout << "Enter The Tablesize: ";
            std::cin >> tablesize;
            
            //get the number of elements
            std::cout << "Enter The Number Of Elements: ";
            std::cin >> num_of_elements;
            
            //check for tards
            if(tablesize < num_of_elements)
                std::cout << "Test Not Valid" << std::endl;
            
        } while(tablesize < num_of_elements);
        
        // Seed the random generator with the current time
        srand(time(NULL));
        
        // Time the experiment
        struct timeval start,end;
        gettimeofday(&start,NULL);
        
        // Run the experiment (tablesize = 1M elements, generate 10k elements)
        RunExperiment(tablesize, num_of_elements);
        
        // Print the runtime
        gettimeofday(&end,NULL);
        fprintf(stdout,"Experiment Runtime: %f\n\n",(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000.0);
        
        testnumber++;
    } while(tablesize != 0  && num_of_elements != 0);
    
    return 0;
}
