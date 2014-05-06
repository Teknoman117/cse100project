//
//  HashTable.hpp
//  CSE 100 Extra Credit Project 1
//
//  Created by Nathaniel Lewis, Matthew Bucci, Zachary Canann on 4/28/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#ifndef __HASHTABLE_HPP_
#define __HASHTABLE_HPP_

#include <vector>
#include <limits>
#include <algorithm>

// Class to manage a hash table (T = type, M = length, H = hash functor)
template <typename T, typename H>
class HashTable
{
public:
    // Define the storage type of the array
    typedef std::vector<T> storage_type;
    
    // Define the search result type
    typedef typename storage_type::iterator search_result;
    
    // Special sentinel values for hash entries
    typedef enum : T
    {
        // The location has never been used
        ENTRY_UNUSED = -2,
        
        // The location has contained data and has been deleted
        ENTRY_EMPTY = -1,
    } Sentinel;
    
protected:
    // data storage for the hash table
    storage_type storage;
    
    // hash function for the hash table
    H hash_function;
    
    // number of entries in the table
    typename storage_type::size_type M;
    
public:
    // Constructor - initializes the hash function and initializes the storage
    HashTable(typename storage_type::size_type M)
        : storage(M, ENTRY_UNUSED), hash_function(H(M)), M(M)
    {
        
    }
    
    // Insert an element into the hash table
    bool insert(const T& k)
    {
        // Locate a place to insert the key
        for(int i = 0; i < M; i++)
        {
            // Calculate the iterator for this entry
            typename storage_type::iterator it = storage.begin() + hash_function(k, i);

            // Check if this spot is empty or unused
            if(*it == ENTRY_EMPTY || *it == ENTRY_UNUSED)
            {
                // We can insert the value here
                *it = k;
                
                // key inserted successfully
                return true;
            }
        }

        // Hash table is full
        return false;
    }
    
    // Delete an element form the hash table
    void remove(const T k)
    {
        // Get an iterator to the proper chain
        search_result result = search(k);
        
        // Remove k if it exists
        if(result != storage.end())
        {
            // Write empty into this iterator
            *result = ENTRY_EMPTY;
        }
    }
    
    // Search the hash table for an element
    search_result search(const T& k)
    {
        // have an iterator
        search_result it;
        
        // Keep hashing, looking for the entry (M is constant, compiler will unwind)
        for(int i = 0; i < M; i++)
        {
            // Calculate the iterator for the target entry
            it = storage.begin() + hash_function(k, i);
        
            // Is the spot we found unused?
            if(*it == ENTRY_UNUSED)
            {
                // Return not found, as there are no more entries to check
                return storage.end();
            }
            
            // Are these the droids we're looking for?
            else if(*it == k)
            {
                // Return the iterator to the location in the storage
                return it;
            }
        }
        
        // Not found, return the end of the hash table
        return storage.end();
    }
    
    // Return the not found index of the hash table
    search_result NotFound()
    {
        return storage.end();
    }
    
    // Get the integral index of a returned storage iterator
    typename storage_type::size_type indexOf(search_result& it)
    {
        // Validate the iterator pair
        if(it == storage.end())
        {
            return std::numeric_limits<typename storage_type::size_type>::max();
        }
        
        // Return the conversion (first int is the index of the hash entry, second is the index within that chain)
        return std::distance(storage.begin(), it);
    }
};

#endif
