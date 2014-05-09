//
//  HashTable.hpp
//  CSE 100 Extra Credit Project 1
//
//  Created by Nathaniel Lewis, Matthew Bucci, Zachary Canann on 4/28/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#ifndef __HASHFUNCTIONS_HPP_
#define __HASHFUNCTIONS_HPP_

#include <cmath>
#include <cstdint>
#include <algorithm>

// Basic hash function with linear probing ((k mod m) + i) mod m
template <typename T>
struct hash
{
    // Size of the parent hash table
    size_t M;
    
    // Constructor
    hash(size_t M)
    {
        this->M = M;
    }
    
    // Functor operator
    size_t operator() (const T& k, const T& i) const
    {
        // Basic hash
        return ((k % M) + i) % M;
    }
};

// Basic hash function with dual hashing ((k mod m) + (2k+1) mod m) mod m
template <typename T>
struct hash_double
{
    // Size of the parent hash table
    size_t M;
    
    // Constructor
    hash_double(size_t M)
    {
        this->M = M;
    }
    
    // Functor operator
    size_t operator() (const T& k, const T& i) const
    {
        // Basic hash
        return ((k % M) + ((i*(2*k+1)) % M)) % M;
    }
};

// Multiplicative hash function with linear probing
template <typename T>
struct hash_mult
{
    // Size of the parent hash table
    float fM;
    size_t M;
    float A;
    
    // Constructor
    hash_mult(size_t M)
    {
        this->M = M;
        this->fM = (float) M;
        A = (std::sqrt(5.f) - 1.0f) / 2.0f;
    }
    
    // Functor operator
    size_t operator() (const T& k, const T& i) const
    {
        // Calculate kA
        float kA = A * (float) k;
        
        // Return the hash
        return ((size_t) floor(fM * (kA - floor(kA))) + i) % M;
    }
};

// Multiplicative hash function with double hashing
template <typename T>
struct hash_mult_double
{
    // Size of the parent hash table
    float fM;
    size_t M;
    float A;
    
    // Constructor
    hash_mult_double(size_t M)
    {
        this->M = M;
        this->fM = (float) M;
        A = (std::sqrt(5.f) - 1.0f) / 2.0f;
    }
    
    // Functor operator
    size_t operator() (const T& k, const T& i) const
    {
        // Calculate kA
        float kA = A * (float) k;
        
        // Return the hash
        return ((size_t) floor(fM * (kA - floor(kA))) + ((i*(2*k+1)) % M)) % M;
    }
};

// Basic hash function (k mod m)
template <typename T>
struct chash
{
    // Size of the parent hash table
    size_t M;
    
    // Constructor
    chash(size_t M)
    {
        this->M = M;
    }
    
    // Functor operator
    size_t operator() (const T& k) const
    {
        // Basic hash
        return k % M;
    }
};

#endif
