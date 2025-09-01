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

#include "utils_hash.h"

int hash_init( hash_ctx *ctx ) {
    shake256_init( ctx );
    return 0;
}

int hash_update( hash_ctx *ctx, const unsigned char *mesg, size_t mlen ) {
    shake256_absorb( ctx, mesg, mlen );
    return 0;
}

int hash_ctx_copy( hash_ctx *nctx, const hash_ctx *octx ) {
    memcpy( nctx, octx, sizeof(hash_ctx) );
    return 0;
}

int hash_final_digest( unsigned char *out, size_t outlen, hash_ctx *ctx ) {
    shake256_finalize( ctx );

    unsigned nblocks = outlen / SHAKE256_RATE;
    shake256_squeezeblocks(out, nblocks, ctx);
    outlen -= nblocks * SHAKE256_RATE;
    out += nblocks * SHAKE256_RATE;
    shake256_squeeze(out, outlen, ctx);

    return 0;
}

