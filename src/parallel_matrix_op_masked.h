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

#ifndef PARALLEL_MATRIX_OP_H
#define PARALLEL_MATRIX_OP_H

#include "UOV_params.h"
#include "UOV_params_masking.h"
#include "gf256.h"
#include "mask_util.h"
#include "refresh.h"
#include "settings.h"
#include "UOV_params.h"
#include "UOV_params_masking.h"
#include <stdint.h>
#include <stddef.h>

void secDotProd_gf256(uint8_t z[N_SHARES], uint8_t x[UOV_L][N_SHARES], uint8_t y[UOV_L][N_SHARES]);
void secMatVec_gf256(uint8_t b[UOV_M][N_SHARES], uint8_t A[UOV_M][UOV_L][N_SHARES], uint8_t x[UOV_L][N_SHARES]);
void secMatVec_stop_early_gf256(uint8_t b[UOV_M][N_SHARES], uint8_t A[UOV_M][UOV_L][N_SHARES], uint8_t x[UOV_L][N_SHARES], uint8_t stop);
void secQuad_gf256(uint8_t y[UOV_M][N_SHARES], uint8_t P[UOV_M][UOV_L][UOV_L], uint8_t x[UOV_L][N_SHARES]);

#endif
