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

#ifndef OV_H
#define OV_H

#include "UOV_params_masking.h"
#include "UOV_params.h"
#include "utils_hash.h"
#include "utils_hash_masked.h"
#include "parallel_matrix_op_masked.h"
#include "blas_matrix_masked.h"
#include "refresh.h"
#include "ov_keypair_masked.h"
#include "utils_prng.h"
#include <stdint.h>
#include <string.h>
#include <stdint.h>

int generate_keypair_pkc_masked(cpk_masked_t *pk, sk_masked_t *sk, sk_seed_t *sk_seed);
int ov_sign_masked( uint8_t *signature, const sk_masked_t *sk, const uint8_t *message, uint32_t mlen );

#endif
