#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "da_rand.h"

static uint64_t xstate[4];

/**
    Rotates bits of a 64-bit word k-number of times to the left.
*/
static uint64_t h_rol64(uint64_t x, uint64_t k)
{
    return (x << k) | (x >> (64 - k));
}

/**
    Generates a random 64-bit number.
*/
uint64_t da_rand64()
{
    uint64_t result = h_rol64(xstate[1] * 5, 7) * 9;
    uint64_t t = xstate[1] << 17;

    xstate[2] ^= xstate[0];
    xstate[3] ^= xstate[1];
    xstate[1] ^= xstate[2];
    xstate[0] ^= xstate[3];

    xstate[2] ^= t;
    xstate[3] = h_rol64(xstate[3], 45);

    return result;
}

/**
    Generates a random 32-bit number.
*/
uint32_t da_rand32()
{
    static int64_t random_sample = 0;
    static uint8_t sample_counter = 0;
    uint32_t output;

    if (sample_counter == 0)
    {
        random_sample = da_rand64();
        sample_counter = 2;
    }

    output = (uint32_t)random_sample;
    random_sample >>= 32;
    --sample_counter;

    return output;
}

/**
    Generates a random 16-bit number.
*/
uint16_t da_rand16()
{
    static int64_t random_sample = 0;
    static uint8_t sample_counter = 0;
    uint16_t output;

    if (sample_counter == 0)
    {
        random_sample = da_rand64();
        sample_counter = 4;
    }

    output = (uint32_t)random_sample;
    random_sample >>= 16;
    --sample_counter;

    return output;
}

/**
    Random number generator used exclusively for initializing
    the state of the xoshiro256** RNG.
*/
static uint64_t h_splitmix64(uint64_t x)
{
    uint64_t z = (x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

/**
    Generates a normalized random single-precision float.
*/
float da_randf(int use_negative)
{
    static const uint32_t mask_exponent = 0x3f800000; // mask for exponent
    static const uint32_t mask_po = 0x3fffffff;       // positive only
    static const uint32_t mask_pn = 0xbfffffff;
    static uint64_t random_sample = 0;
    static uint8_t sample_counter = 0;
    uint32_t i;
    float output;

    if (sample_counter == 0)
    {
        random_sample = da_rand64();
        sample_counter = 2;
    }

    // Apply masks
    i = (uint32_t)random_sample | mask_exponent;
    i &= use_negative == 0 ? mask_po : mask_pn;

    memcpy(&output, &i, 4); // float assumed to be 32-bit
    output += output > 0 ? -1 : 1;

    random_sample >>= 23;
    --sample_counter;

    return output;
}

/**
        Generates a random single-precision float between two real numbers.
*/
float da_randf_range(float min, float max)
{
    return (da_randf(0) * (max - min)) + min;
}

/**
    Initializes the random state.
*/
void da_initrstate(uint64_t rseed)
{
    rseed = h_splitmix64(rseed);
    xstate[0] = rseed;
    xstate[1] = rseed;
    xstate[2] = rseed;
    xstate[3] = rseed;
    da_rand64();
}
