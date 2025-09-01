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

#ifndef GF256_H
#define GF256_H

#include <stdint.h>

// gf256 := gf2[X]/ (x^8+x^4+x^3+x+1)   // 0x11b , AES field
static inline uint8_t gf256_mul(uint8_t a, uint8_t b) {
    uint8_t r = a * (b & 1);

    a = (a << 1) ^ ((a >> 7) * 0x1b);
    r ^= a * ((b >> 1) & 1);
    a = (a << 1) ^ ((a >> 7) * 0x1b);
    r ^= a * ((b >> 2) & 1);
    a = (a << 1) ^ ((a >> 7) * 0x1b);
    r ^= a * ((b >> 3) & 1);
    a = (a << 1) ^ ((a >> 7) * 0x1b);
    r ^= a * ((b >> 4) & 1);
    a = (a << 1) ^ ((a >> 7) * 0x1b);
    r ^= a * ((b >> 5) & 1);
    a = (a << 1) ^ ((a >> 7) * 0x1b);
    r ^= a * ((b >> 6) & 1);
    a = (a << 1) ^ ((a >> 7) * 0x1b);
    r ^= a * ((b >> 7) & 1);
    return r;
}

static inline uint8_t gf256_squ(uint8_t a) {
    uint8_t r8 = a & 1;
    r8 ^= (a << 1) & 4;    // x^1 -> x^2
    r8 ^= (a << 2) & (1 << 4); // x^2 -> x^4
    r8 ^= (a << 3) & (1 << 6); // x^3 -> x^6

    r8 ^= ((a >> 4) & 1) * 0x1b; // x^4 -> x^8  --> 0x1b
    r8 ^= ((a >> 5) & 1) * (0x1b << 2); // x^5 -> x^10  --> (0x1b<<2)
    r8 ^= ((a >> 6) & 1) * (0xab); // x^6 -> x^12  --> (0xab)
    r8 ^= ((a >> 7) & 1) * (0x9a); // x^7 -> x^14  --> (0x9a)

    return r8;
}

static inline uint8_t gf256_inv(uint8_t a) {
    // extended GCD
    uint16_t f = 0x11b;
    uint16_t g = ((uint16_t)a) << 1;
    int16_t delta = 1;

    uint16_t r = 0x100;
    uint16_t v = 0;

    for (int i = 0; i < 15; i++) {
        uint16_t g0 = (g >> 8) & 1;
        uint16_t minus_delta = -delta;
        uint16_t swap = (minus_delta >> 15) & g0; // >>15 -> get sign bit
        //uint16_t f0g0 = g0;  // f0 is always 1

        // update delta
        delta = swap * (minus_delta << 1) + delta + 1;

        // update f, g, v, r
        uint16_t f_g = (f ^ g);
        g ^= (f * g0);
        f ^= (f_g) * swap;

        uint16_t v_r = (v ^ r);
        r ^= (v * g0);
        v ^= (v_r) * swap;

        g <<= 1;
        v >>= 1;
    }
    return v & 0xff;
}


#endif
