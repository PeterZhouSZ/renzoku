#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <ctime>
using namespace std;

#include <cstdlib>

#define MT_GENERATE_CODE_IN_HEADER 1
// need to inline because my Random function is inline.

extern "C" {    // mtwist is a C module used in a C++ program
    #include "mtwist.h"
}

namespace Renzoku {
class Random {
public:
    Random() {        
        //mt_seed();
        mt_seed32(2013);
    }

    Random(uint32_t _seed) {     
        mt_seed32(_seed);
    }

    /**
     * Generate random value in [0, 1)
     *
     * We don't want value 1 since it can generate singular cases
     * (e.g., cosine sampling will result in pdf = 0 -> division by zero in Monte Carlo).
     */
    inline Float operator()();
    
    inline Float rand_exclude_01();

public:    
    static const uint32_t  FLOAT_MASK = 0x00ffffff;
    static const uint32_t  TWO_POW_24 = 16777216;
    static const uint32_t  TWO_POW_25 = 33554432;
};
static const Float     INV_TWO_POW_24 = Float(1) / Random::TWO_POW_24;
static const Float     INV_TWO_POW_25 = Float(1) / Random::TWO_POW_25;

/**
 * Note that if we map 32-bit unsigned integers [0, 2^32) to [0, 1), 
 * it is necessary to use double to store the value [0, 1) since it is generated by 
 * dividing a random 32-bit unsigned integer by 2^32 (exclude 1) or 2^32 - 1 (include 1). 
 * Using IEEE 754 32-bit floating point (float), 2^32 and 2^32-1 have the *same* representation, 
 * which causes the value 1 to appear (float x = (2^32-1) / 2^32 = 1.0f!!!). 
 *
 * Therefore, 64-bit floating point (double) is required.
 *
 * Another workaround is to reduce the resolution of the random numbers by only generating 
 * integers from [0, 2^24) so 32-bit floating point is enough to store these random numbers. 
 *
 * This is also the approach taken by PBRT and LuxRender. 
 */
inline Float Random::operator()() {
    uint32_t r = mt_lrand();
    return (r & FLOAT_MASK) * INV_TWO_POW_24;    
}

inline Float Random::rand_exclude_01() {
    uint32_t r = mt_lrand();

    // NOTE: this hack is from myself to make use of existing mt_lrand() which generates number from 0 to 2^32-1. 
    // Not sure if this is a standard approach to generate (0, 1) (exclusive).
    return (r & FLOAT_MASK) * INV_TWO_POW_24 + INV_TWO_POW_25; // shift to (0, 1) by + 0.5 into the random integer
}

/*
inline double Random::operator()() {
    //return mt_drand(); // 32-bit long integer to double 
    double val = mt_drand();
    if (val == 1.0) {
        printf("Inclusive!!!\n");
    }
    return val;
}*/

}; // end namespace Renzoku

#endif
