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
#include <array>
#include <cstdint>
#include <cassert>
#include <fstream>
#include <iomanip>

#include "Utilities.h"
#include "OS.hpp"

#include "HashTable.hpp"
#include "HashFunctions.hpp"
#include "ChainingHashTable.hpp"

// Time the execution of a function
double TimeExecution(std::function<void ()> f)
{
    double s = OS::Now();
    f();
    return OS::Now() - s;
}

// Generate the keys for the experiment
void GenerateKeys(size_t m, size_t n, std::set<int64_t>& S, std::set<int64_t>& D, std::set<int64_t>& U)
{
    // Clear the provided sets
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

struct ExperimentResult
{
    // Runtimes of experiments
	double tS, tU, ptS, ptU;
    
    // Information for CSV files
    double alpha;
    
    // Name of hash function
    std::string hashFunctionName;
    
    // Write the headers for the CSV
    static void WriteCSVHeader(std::ofstream& file)
    {
        file << "Hash Function" << ",";
        file << "Alpha" << ",";
        file << "Tsuccessful" << ",";
        file << "Tfailed" << ",";
        file << "Tsuccessful\'" << ",";
        file << "Tfailed\'";
        file << std::endl;
    }
    
    // Write the experiment contents
    void WriteCSVEntry(std::ofstream& file)
    {
        file << hashFunctionName << ",";
        file << std::setprecision(2)  << alpha << ",";
        file << std::setprecision(10) << std::fixed << tS << ",";
        file << std::setprecision(10) << std::fixed << tU << ",";
        file << std::setprecision(10) << std::fixed << ptS << ",";
        file << std::setprecision(10) << std::fixed << ptU << ",";
        file << std::endl;
    }
	
    // Stuff
    double minTime, maxTime;
	std::string minType, maxType;
	void calcMin()
	{
		minTime = tS;
		minType = "tS";

		if (tU < minTime)
		{
			minTime = tU;
			minType = "tU";
		}

		if (ptS < minTime)
		{
			minTime = ptS;
			minType = "ptS";
		}

		if (ptU < minTime)
		{
			minTime = ptU;
			minType = "ptU";
		}
	}

	void calcMax()
	{
		maxTime = tS;
		maxType = "tS";

		if (tU > maxTime)
		{
			maxTime = tU;
			maxType = "tU";
		}

		if (ptS > maxTime)
		{
			maxTime = ptS;
			maxType = "ptS";
		}

		if (ptU > maxTime)
		{
			maxTime = ptU;
			maxType = "ptU";
		}
	}

	void print()
	{
		calcMin();
		calcMax();

		// Results
		std::cout << "tS = " << tS << ", tU = " << tU << std::endl;
		std::cout << "tU / tS = " << tU / tS << std::endl;
		std::cout << "tS' = " << ptS << ", tU' = " << ptU << std::endl;
		std::cout << "tU' / tS' = " << ptU / ptS << std::endl;

		std::cout << "Min is " << minType.c_str() << " with time:\t" << minTime << std::endl;
		std::cout << "Max is " << maxType.c_str() << " with time:\t" << maxTime << std::endl;
	}
};

// Run the experiment
template <class T>
void RunExperiment(std::ofstream& logfile, size_t m, double a)
{
    // Datasets for the experiment
    std::set<int64_t> S;
    std::set<int64_t> D;
    std::set<int64_t> U;

    // Experiement result management
	ExperimentResult result;
    result.alpha = a;
    result.hashFunctionName = "basic";
    
    // Generate keys (force n as even)
    size_t n = m * a;
    if(n % 2) n--;
    GenerateKeys(m, n, S, D, U);
    
    // Create a hash table w/ open indexing for this lab
    HashTable<int64_t,T> t(m);
    
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
    result.tS = TimeExecution( [&] ()
    {
        for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
        {
            // Perform the search
            typename HashTable<int64_t, T>::search_result r = t.search(*it);
            assert(*r == *it);
        }
    });
    result.tS /= S.size();
    
    
    // Search for the elements in the U set
	result.tU = TimeExecution( [&] ()
    {
        for(std::set<int64_t>::iterator it = U.begin(); it != U.end(); it++)
        {
            // Perform the search
            typename HashTable<int64_t,T>::search_result r = t.search(*it);
            assert(r == t.NotFound());
        }
    });
    result.tU /= U.size();
    
    
    // Get the clusters
    /*typename HashTable<int64_t,T>::cluster_type clusters;
    t.clusters(clusters);
    
    // Print the clusters
    size_t count = 0;
    for(typename HashTable<int64_t,T>::cluster_type::iterator cluster = clusters.begin(); cluster != clusters.end(); cluster++)
    {
        std::cout << "Cluster Size: " << cluster->first << "; Frequency: " << cluster->second << std::endl;
        count += cluster->first * cluster->second;
    }
    std::cout << "Total Results = " << count << std::endl << std::endl;*/
    
    // Remove the elements from the D set
    for(std::set<int64_t>::iterator it = D.begin(); it != D.end(); it++)
    {
        t.remove(*it);
    }
    
    // Perform the secondary experiment
    // Search for the elements in the S set
	result.ptS = TimeExecution( [&] ()
    {
        for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
        {
            // Perform the search
            typename HashTable<int64_t,T>::search_result r = t.search(*it);
            assert(*r == *it);
        }
	});
    result.ptS /= S.size();
    
    
    // Search for the elements in the U set
	result.ptU = TimeExecution( [&] ()
    {
        for(std::set<int64_t>::iterator it = U.begin(); it != U.end(); it++)
        {
            // Perform the search
            typename HashTable<int64_t,T>::search_result r = t.search(*it);
            assert(r == t.NotFound());
        }
	});
    result.ptU /= U.size();
    
    // Get the clusters
    /*t.clusters(clusters);
    
    // Print the clusters
    count = 0;
    for(typename HashTable<int64_t,T>::cluster_type::iterator cluster = clusters.begin(); cluster != clusters.end(); cluster++)
    {
        std::cout << "Cluster Size: " << cluster->first << "; Frequency: " << cluster->second << std::endl;
        count += cluster->first * cluster->second;
    }
    std::cout << "Total Results = " << count << std::endl << std::endl;*/
    result.WriteCSVEntry(logfile);
    result.print();
}

// Main method
int main(int argc, const char * argv[])
{
    // Seed the random generator with some dead beef
    srand(0xDEADBEEF);
    
    // Open the results file
    std::ofstream logfile;
    logfile.open("results.csv");
    ExperimentResult::WriteCSVHeader(logfile);
    std::cout << "--> Writing results to \"results.csv\" <--" << std::endl;
    
    // Possible alpha values
    std::array<double, 6> alphas = {0.05, 0.10, 0.50, 0.80, 0.90, 0.95};
    int tablesize = 1048576;
    
    // Run experiments
    for(std::array<double, 6>::iterator alpha = alphas.begin(); alpha != alphas.end(); alpha++)
    {
        // Log this data
        std::cout << "Starting Experiment: Alpha = " << *alpha << std::endl;
        
        // Time the experiment
        double runtime = TimeExecution([&] ()
        {
            RunExperiment<hash<int64_t> >(logfile, tablesize, *alpha);
        });
        
        // Log this data
        std::cout << "Completed Experiment in " << runtime << " seconds" << std::endl;
    }
    
    // Close the log
    logfile.close();
    std::cout << "--> Log closed <--" << std::endl;
    
    return 0;
}
