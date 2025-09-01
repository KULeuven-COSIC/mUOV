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

#include "fips202_masked.h"
#include "utils_hash_masked.h"
#include <stdint.h>
#include <string.h>

int hash_init_masked(mhash_ctx *ctx) {
    shake256_init_masked(ctx);
    return 0;
}

int hash_update_masked(mhash_ctx *ctx, const unsigned char *mesg, size_t mlen) {
    shake256_absorb_masked(ctx, mesg, mlen);
    return 0;
}

int hash_ctx_copy_masked(mhash_ctx *nctx, const mhash_ctx *octx) {
    memcpy(nctx, octx, sizeof(mhash_ctx));
    return 0;
}

int hash_final_digest_masked(unsigned char *out, size_t outlen, mhash_ctx *ctx ) {
    shake256_finalize_masked(ctx);

    unsigned nblocks = outlen / SHAKE256_RATE;
    shake256_squeezeblocks_masked(out, nblocks, ctx);
    outlen -= nblocks * SHAKE256_RATE;
    out += nblocks * SHAKE256_RATE*N_SHARES;
    shake256_squeeze_masked(out, outlen, ctx);
    return 0;
}
