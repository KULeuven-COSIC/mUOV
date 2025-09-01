// MIT License

// Copyright (c) 2025 KU Leuven - COSIC

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "mask_util.h"

/**
 * Wrapper over rng_get_random_blocking() or rand()
 */
uint8_t random_uint8()
{
    #ifdef M4_TARGET
    uint8_t rand = rng_get_random_blocking()&0xff;
    #endif
    #ifdef CPU_TARGET
    uint8_t rand, rand_temp[1];
    randombytes(rand_temp,1);
    rand = rand_temp[0];
    #endif

    return rand;
}

#ifdef M4_TARGET
uint16_t random_uint16()
{
    uint16_t rand = rng_get_random_blocking()&0xffff;

    return rand;
}

uint32_t random_uint32()
{
    uint32_t rand = rng_get_random_blocking();

    return rand;
}


int randombytes(uint8_t *obuf, size_t len)
{
    union
    {
        unsigned char aschar[4];
        uint32_t asint;
    } random;

    while (len > 4)
    {
        random.asint = rng_get_random_blocking();
        *obuf++ = random.aschar[0];
        *obuf++ = random.aschar[1];
        *obuf++ = random.aschar[2];
        *obuf++ = random.aschar[3];
        len -= 4;
    }
    if (len > 0)
    {
        for (random.asint = rng_get_random_blocking(); len > 0; --len)
        {
            *obuf++ = random.aschar[len - 1];
        }
    }

    return 0;
}

#endif

#ifdef CPU_TARGET
void randombytes(unsigned char *x, unsigned long long xlen) {
    while (xlen--) {
        *x++ = rand() & 0xff;
    }
}
#endif
