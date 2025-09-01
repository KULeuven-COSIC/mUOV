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

#ifndef UTILS_HASH_MASKED_H
#define UTILS_HASH_MASKED_H

#include "fips202_masked.h"
#include <stdint.h>
#include <stddef.h>

typedef keccak_state_masked mhash_ctx;

int hash_init_masked(mhash_ctx *ctx);

int hash_update_masked(mhash_ctx *ctx, const unsigned char *mesg, size_t mlen);

int hash_ctx_copy_masked(mhash_ctx *nctx, const mhash_ctx *octx);     // nctx needs no hash_init()

int hash_final_digest_masked(unsigned char *digest, size_t dlen, mhash_ctx *ctx);     // free ctx

#endif
