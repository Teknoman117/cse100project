//
//  HashTable.hpp
//  CSE 100 Extra Credit Project 1
//
//  Created by Nathaniel Lewis, Matthew Bucci, Zachary Canann on 4/28/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#ifndef __CHAININGHASHTABLE_HPP_
#define __CHAININGHASHTABLE_HPP_

#include <vector>
#include <limits>
#include <algorithm>
#include <list>

// Class to manage a hash table (T = type, H = hash functor)
template <typename T, typename H>
class ChainingHashTable
{
public:
    // Define the storage type of the array
    typedef std::vector<std::list<T> > storage_type;
    
    // Search result
    typedef std::pair<bool, typename std::list<T>::iterator> search_result;
    
protected:
    // data storage for the hash table
    storage_type storage;
    
    // hash function for the hash table
    H hash_function;
    
    // number of entries in the table
    typename storage_type::size_type M;
    
public:
    // Constructor - initializes the hash function and initializes the storage
    ChainingHashTable(typename storage_type::size_type M)
        : storage(M), hash_function(H(M)), M(M)
    {
        
    }
    
    // Insert an element into the hash table
    void insert(const T& k)
    {
        // Get an iterator to the container cooresponding to this key
        typename storage_type::iterator it = storage.begin() + hash_function(k);
        
        // Put the key onto the chain
        it->push_front(k);
    }
    
    // Delete an element form the hash table
    void remove(const T& k)
    {
        // Get an iterator to the container cooresponding to this key
        typename storage_type::iterator it = storage.begin() + hash_function(k);
        
        // Get an iterator to the key in this hash table
        typename std::list<T>::iterator kIt = it->find(k);
        
        // If the key was found, delete it
        if(kIt != it->end())
        {
            // Delete this key
            it->erase(kIt);
        }
    }
    
    // Search the hash table for an element
    search_result search(const T& k)
    {
        // Get an iterator to the container cooresponding to this key
        typename storage_type::iterator it = storage.begin() + hash_function(k);
        
        // Get an iterator to the key in this hash table
        typename std::list<T>::iterator kIt = it->find(k);
        
        // If the key was found, return it
        if(kIt != it->end())
        {
            return search_result(true, kIt);
        }
        
        // Otherwise return sadness and broken dreams
        return search_result(false, kIt);
    }
};

#endif
