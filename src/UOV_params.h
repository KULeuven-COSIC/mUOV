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

#ifndef UOV_PARAMS_H
#define UOV_PARAMS_H

#include "UOV_params_masking.h"

#define UOV_GFSIZE 256
#define _4ROUND_AES_

/////////////////////////////////////////////////////
// Use pre-defined parameter sets, or define custom (below)

#define UOV_I
// #define UOV_III
// #define UOV_V
// #define CUSTOM / USE FOR M4_TARGET

////////////////////////////////////////////////////

#ifdef UOV_I
#define UOV_M 44
#define UOV_N 112
// #define UOV_EARLY_STOP
// #define UOV_STOP 16
#define UOV_HASH_BYTE 32
#endif

#ifdef UOV_III
#define UOV_M 72
#define UOV_N 184
// #define UOV_EARLY_STOP
// #define UOV_STOP 16
#define UOV_HASH_BYTE 48
#endif

#ifdef UOV_V
#define UOV_M 96
#define UOV_N 244
// #define UOV_EARLY_STOP
// #define UOV_STOP 16
#define UOV_HASH_BYTE 64
#endif

#ifdef CUSTOM
#define UOV_M 11
#define UOV_N 28
// #define UOV_EARLY_STOP
// #define UOV_STOP 3
#define UOV_HASH_BYTE 32
#endif

#define UOV_M_BYTE UOV_M
#define UOV_N_BYTE UOV_N
#define UOV_L (UOV_N - UOV_M)

#define UOV_O (UOV_M)
#define UOV_O_BYTE UOV_O
#define UOV_V ((UOV_N) - (UOV_O))
#define UOV_V_BYTE UOV_V

#define UOV_O_matrix_BYTE (UOV_L*UOV_M)

#define UOV_PKSEED_BYTE 16
#define UOV_SKSEED_BYTE 32 
#define UOV_SALT_BYTE 16

#define UOV_SIGNATUREBYTES (UOV_N_BYTE + UOV_SALT_BYTE)

#define UOV_N_TRIANGLE_TERMS(n_var) ((n_var) * ((n_var) + 1) / 2)

#define UOV_PK_BYTE (UOV_M_BYTE * UOV_N_TRIANGLE_TERMS(UOV_N))
#define UOV_PK_P1_BYTE (UOV_O_BYTE * UOV_N_TRIANGLE_TERMS(UOV_V))
#define UOV_PK_P2_BYTE (UOV_O_BYTE * UOV_V * UOV_O)
#define UOV_PK_P3_BYTE (UOV_O_BYTE * UOV_N_TRIANGLE_TERMS(UOV_O))

#define UOV_SK_UNCOMPRESSED_BYTE (N_SHARES * UOV_SKSEED_BYTE + \
                                  N_SHARES * UOV_V_BYTE * UOV_O + \
                                  UOV_O_BYTE * UOV_N_TRIANGLE_TERMS(UOV_V) + \
                                  N_SHARES * UOV_O_BYTE * UOV_V * UOV_O) 
#define UOV_PK_COMPRESSED_BYTE (UOV_PKSEED_BYTE + UOV_O_BYTE * UOV_N_TRIANGLE_TERMS(UOV_O))

#define UOV_SECRETKEYBYTES UOV_SK_UNCOMPRESSED_BYTE
#define UOV_PUBLICKEYBYTES UOV_PK_COMPRESSED_BYTE

#endif
