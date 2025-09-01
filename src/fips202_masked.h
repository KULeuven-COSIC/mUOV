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

#ifndef FIPS202_MASKED_H
#define FIPS202_MASKED_H

#include "UOV_params_masking.h"
#include <stddef.h>
#include <stdint.h>

#define SHAKE256_RATE 136

typedef struct {
    uint64_t s[25][N_SHARES];
    unsigned int pos;
} keccak_state_masked;

void shake256_init_masked(keccak_state_masked *state);
void shake256_absorb_masked(keccak_state_masked *state, const uint8_t *in, size_t inlen);
void shake256_finalize_masked(keccak_state_masked *state);
void shake256_squeeze_masked(uint8_t *out, size_t outlen, keccak_state_masked *state);
void shake256_squeezeblocks_masked(uint8_t *out, size_t nblocks, keccak_state_masked *state);

#endif
