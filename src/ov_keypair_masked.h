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

#ifndef OV_KEYPAIR_H
#define OV_KEYPAIR_H

#include "UOV_params_masking.h"
#include "UOV_params.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/// alignment 1 for struct
#pragma pack(push,1)

typedef
struct {
    unsigned char pk_seed[UOV_PKSEED_BYTE];         
    unsigned char P3[UOV_PK_P3_BYTE];
} cpk_masked_t;

typedef
struct {
    unsigned char sk_seed[UOV_SKSEED_BYTE][N_SHARES];
    unsigned char O[UOV_L][UOV_M][N_SHARES];
    unsigned char P1[UOV_PK_P1_BYTE];
    unsigned char S[UOV_M][UOV_L][UOV_M][N_SHARES];
} sk_masked_t;

typedef
struct {
    unsigned char sk_seed[UOV_SKSEED_BYTE][N_SHARES];
} sk_seed_t;

/// restores alignment
#pragma pack(pop)

#endif
