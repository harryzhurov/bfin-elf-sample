//*-----------------------------------------------------------------------------
//*
//*   File: Configuration for Zltigo heap manager
//*
//*   Description: Contains guard for thread-safe heap management.
//*                The guard is the object with lock()/unlock()
//*                functions pair
//*
//*-----------------------------------------------------------------------------


#ifndef HEAP_CFG_H
#define HEAP_CFG_H

//------------------------------------------------------------------------------
//                 
//   Guard type for single-thread applications              
//                 
struct heap_no_guard
{
    void lock()   { }
    void unlock() { }
};
//------------------------------------------------------------------------------
typedef heap_no_guard heap_guard;

//------------------------------------------------------------------------------
#endif // HEAP_CFG_H













