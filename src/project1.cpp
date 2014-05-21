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
#include <sstream>

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
        file << "Load Factor" << ",";
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
    
    // Write clusters
    template <class T>
    void WriteClusterCSV(HashTable<int64_t,T>& t)
    {
        // Get the clusters
        typename HashTable<int64_t,T>::cluster_type clusters;
        t.clusters(clusters);

        // Open file
        std::ofstream logfile;
        std::ostringstream logfileName;
        logfileName << hashFunctionName << "." << alpha << ".csv" << std::ends;
        logfile.open(logfileName.str().c_str());
        
        // Print the clusters
        logfile << "Cluster Length,Cluster Frequency" << std::endl;
        for(typename HashTable<int64_t,T>::cluster_type::iterator cluster = clusters.begin(); cluster != clusters.end(); cluster++)
        {
            logfile << cluster->first << "," << cluster->second << std::endl;
        }
        
        // Close the logfile
        logfile.close();
    }
};

// Run the experiment
template <class T>
void RunExperiment(std::ofstream& logfile, std::string name, size_t m, double a)
{
    // Datasets for the experiment
    std::set<int64_t> S;
    std::set<int64_t> D;
    std::set<int64_t> U;

    // Experiement result management
	ExperimentResult result;
    result.alpha = a;
    result.hashFunctionName = name;
    
    // Generate keys (force n as even)
    size_t n = static_cast<size_t>(m * a) >> 1;
    n <<= 1;
    //std::cout << "    Load = " << n << std::endl;
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
    //std::cout << "    tS test" << std::endl;
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
    //std::cout << "    tU test" << std::endl;
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
    
    // Write the clusters to CSV
    result.WriteClusterCSV<T>(t);
    
    // Remove the elements from the D set
    //std::cout << "    D set deletion phase" << std::endl;
    for(std::set<int64_t>::iterator it = D.begin(); it != D.end(); it++)
    {
        t.remove(*it);
    }
    
    // Perform the secondary experiment
    // Search for the elements in the S set
    //std::cout << "    tS\' test" << std::endl;
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
    //std::cout << "    tU\' test" << std::endl;
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
    
    // Write the clusters to CSV
    result.WriteClusterCSV<T>(t);
    
    // Write the CSV entry
    result.WriteCSVEntry(logfile);
}

template <class T>
void RunExperimentChaining(std::ofstream& logfile, std::string name, size_t m, double a)
{
    // Datasets for the experiment
    std::set<int64_t> S;
    std::set<int64_t> D;
    std::set<int64_t> U;
    
    // Experiement result management
	ExperimentResult result;
    result.alpha = a;
    result.hashFunctionName = name;
    
    // Generate keys (force n as even)
    size_t n = static_cast<size_t>(m * a) >> 1;
    n <<= 1;
    //std::cout << "    Load = " << n << std::endl;
    GenerateKeys(m, n, S, D, U);
    
    // Create a hash table w/ open indexing for this lab
    ChainingHashTable<int64_t,T> t(m);
    
    // Insert the elements from the S set
    for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
    {
        t.insert(*it);
    }
    
    // Insert the elements from the D set
    for(std::set<int64_t>::iterator it = D.begin(); it != D.end(); it++)
    {
        t.insert(*it);
    }
    
    // Perform the initial experiment
    // Search for the elements in the S set
    //std::cout << "    tS test" << std::endl;
    result.tS = TimeExecution( [&] ()
    {
        for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
        {
            // Perform the search
            typename ChainingHashTable<int64_t, T>::search_result r = t.search(*it);
            assert(*(r.second) == *it);
        }
    });
    result.tS /= S.size();
    
    // Search for the elements in the U set
    //std::cout << "    tU test" << std::endl;
	result.tU = TimeExecution( [&] ()
    {
        for(std::set<int64_t>::iterator it = U.begin(); it != U.end(); it++)
        {
            // Perform the search
            typename ChainingHashTable<int64_t,T>::search_result r = t.search(*it);
            assert(!r.first);
        }
    });
    result.tU /= U.size();
    
    // Remove the elements from the D set
    //std::cout << "    D set deletion phase" << std::endl;
    for(std::set<int64_t>::iterator it = D.begin(); it != D.end(); it++)
    {
        t.remove(*it);
    }
    
    // Perform the secondary experiment
    // Search for the elements in the S set
    //std::cout << "    tS\' test" << std::endl;
	result.ptS = TimeExecution( [&] ()
                               {
                                   for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
                                   {
                                       // Perform the search
                                       typename ChainingHashTable<int64_t,T>::search_result r = t.search(*it);
                                       assert(*(r.second) == *it);
                                   }
                               });
    result.ptS /= S.size();
    
    // Search for the elements in the U set
    //std::cout << "    tU\' test" << std::endl;
	result.ptU = TimeExecution( [&] ()
                               {
                                   for(std::set<int64_t>::iterator it = U.begin(); it != U.end(); it++)
                                   {
                                       // Perform the search
                                       typename ChainingHashTable<int64_t,T>::search_result r = t.search(*it);
                                       assert(!r.first);
                                   }
                               });
    result.ptU /= U.size();
    
    // Write the CSV entry
    result.WriteCSVEntry(logfile);
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
    std::array<double, 8> alphas = {0.05, 0.10, 0.50, 0.80, 0.90, 0.9167, 0.9333, 0.95};
    int tablesize = 1048576;
    
    // Basic hashing
    std::cout << "Testing: Basic Hash" << std::endl;
    for(std::array<double, 8>::iterator alpha = alphas.begin(); alpha != alphas.end(); alpha++)
    {
        double r = TimeExecution([&] () {RunExperiment<hash<int64_t> >(logfile, "basic", tablesize, *alpha);});
        std::cout << "  Alpha " << *alpha << ": " << r << std::endl;
    }
    
    // Basic hashing
    std::cout << "Testing: Chaining Basic Hash" << std::endl;
    for(std::array<double, 8>::iterator alpha = alphas.begin(); alpha != alphas.end(); alpha++)
    {
        double r = TimeExecution([&] () {RunExperimentChaining<chash<int64_t> >(logfile, "cbasic", tablesize, *alpha);});
        std::cout << "  Alpha " << *alpha << ": " << r << std::endl;
    }
    
    // Double hashing
    /*std::cout << "Testing: Double Hash" << std::endl;
    for(std::array<double, 8>::iterator alpha = alphas.begin(); alpha != alphas.end(); alpha++)
    {
        double r = TimeExecution([&] () {RunExperiment<hash_double<int64_t> >(logfile, "double", tablesize, *alpha);});
        std::cout << "  Alpha " << *alpha << ": " << r << std::endl;
    }
    
    // Multiplicative hashing
    /*std::cout << "Testing: Multiplicative Hash" << std::endl;
    for(std::array<double, 6>::iterator alpha = alphas.begin(); alpha != alphas.end(); alpha++)
    {
        double r = TimeExecution([&] () {RunExperiment<hash_mult<int64_t> >(logfile, "multiplicative", tablesize, *alpha);});
        std::cout << "  Alpha " << *alpha << ": " << r << std::endl;
    }*/
    
    // Multiplicative Double hashing
    /*std::cout << "Testing: Multiplicative Double Hash" << std::endl;
    for(std::array<double, 8>::iterator alpha = alphas.begin(); alpha != alphas.end(); alpha++)
    {
        double r = TimeExecution([&] () {RunExperiment<hash_mult_double<int64_t> >(logfile, "multiplicative double", tablesize, *alpha);});
        std::cout << "  Alpha " << *alpha << ": " << r << std::endl;
    }*/
    
    // Close the log
    logfile.close();
    std::cout << "--> Log closed <--" << std::endl;
    
    return 0;
}
