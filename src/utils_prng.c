#include "utils_prng.h"

#include <string.h>  // for memcpy()

//
// Defining prng_set_publicinputs() and aes128ctr_publicinputs()
//
//

#include "aes128_4r_ffs.h"

int prng_set_publicinputs(prng_publicinputs_t *ctx, const unsigned char prng_seed[16]) {
    ctx->used = RNG_OUTPUTLEN;
    ctx->ctr = 0;
    #ifdef _4ROUND_AES_
    aes128_4r_keyschedule_ffs_lut(ctx->key, prng_seed);
    #else
    aes128_keyschedule_ffs_lut(ctx->key, prng_seed);
    #endif
    return 0;
}


static inline uint32_t br_swap32(uint32_t x) {
    x = ((x & (uint32_t)0x00FF00FF) << 8)
        | ((x >> 8) & (uint32_t)0x00FF00FF);
    return (x << 16) | (x >> 16);
}


static
int aes128ctr_publicinputs( unsigned char *out, unsigned nblocks, const unsigned char *n, uint32_t ctr, const prng_publicinputs_t *pctx ) {
    uint32_t ptext0[4];
    uint32_t ptext1[4];
    uint8_t *p0 = (uint8_t *)ptext0;
    uint8_t *p1 = (uint8_t *)ptext1;
    memcpy( p0, n, AES128CTR_NONCELEN );
    memcpy( p1, n, AES128CTR_NONCELEN );

    while (nblocks--) {
        uint32_t c0 = ctr;
        uint32_t c1 = ctr + 1;

        #if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
        ptext0[3] = c0;
        ptext1[3] = c1;
        #else
        ptext0[3] = br_swap32(c0);
        ptext1[3] = br_swap32(c1);
        #endif
        #ifdef _4ROUND_AES_
        aes128_4r_encrypt_ffs(out, out + 16, p0, p1, pctx->key);
        #else
        aes128_encrypt_ffs(out, out + 16, p0, p1, pctx->key);
        #endif
        out += 32;

        uint32_t c2 = ctr + 2;    // XXX: ctr might overflow 32-bit boundary
        uint32_t c3 = ctr + 3;    // For the usage in OV, this is ok for expanding pk only.

        #if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
        ptext0[3] = c2;
        ptext1[3] = c3;
        #else
        ptext0[3] = br_swap32(c2);
        ptext1[3] = br_swap32(c3);
        #endif
        #ifdef _4ROUND_AES_
        aes128_4r_encrypt_ffs(out, out + 16, p0, p1, pctx->key);
        #else
        aes128_encrypt_ffs(out, out + 16, p0, p1, pctx->key);
        #endif
        out += 32;
        ctr += 4;
    }
    return 0;
}

void prng_release_publicinputs(prng_publicinputs_t *ctx){
    //no-op
    (void) ctx;
}






int prng_gen_publicinputs(prng_publicinputs_t *ctx, unsigned char *out, unsigned long outlen) {
    unsigned long long xlen = outlen;
    unsigned long long ready;
    uint8_t nonce[AES128CTR_NONCELEN] = {0};

    if (ctx->used < RNG_OUTPUTLEN) {
        ready = RNG_OUTPUTLEN - ctx->used;
        if (xlen <= ready) {
            ready = xlen;
        }
        memcpy(out, &ctx->buf[ctx->used], ready);
        ctx->used += ready;
        xlen -= ready;
        out += ready;
    }


    if (xlen >= RNG_OUTPUTLEN) {
        uint32_t nblocks = xlen / RNG_OUTPUTLEN;
        aes128ctr_publicinputs(out, nblocks, nonce, ctx->ctr, ctx);
        ctx->ctr += (RNG_OUTPUTLEN / AES128_BLOCKSIZE) * nblocks;
        xlen -= nblocks * RNG_OUTPUTLEN;
        out += nblocks * RNG_OUTPUTLEN;
    }

    if (xlen > 0) {
        aes128ctr_publicinputs(ctx->buf, 1, nonce, ctx->ctr, ctx);
        ctx->ctr += (RNG_OUTPUTLEN / AES128_BLOCKSIZE);
        memcpy(out, ctx->buf, xlen);
        ctx->used = xlen;
    }
    return outlen;
}


void prng_skip_publicinputs(prng_publicinputs_t *ctx, unsigned long outlen) {
    if (ctx->used + outlen <= RNG_OUTPUTLEN ) {
        ctx->used += outlen;
        return;
    }
    outlen -= (RNG_OUTPUTLEN - ctx->used);

    unsigned long n_blocks_skip = outlen / RNG_OUTPUTLEN;
    unsigned long rem = outlen - n_blocks_skip * RNG_OUTPUTLEN;
    uint8_t nonce[AES128CTR_NONCELEN] = {0};
    if (rem) {
        ctx->ctr += n_blocks_skip * (RNG_OUTPUTLEN / AES128_BLOCKSIZE);
        ctx->used = rem;
        aes128ctr_publicinputs(ctx->buf, 1, nonce, ctx->ctr, ctx);
        ctx->ctr += (RNG_OUTPUTLEN / AES128_BLOCKSIZE);
    } else {  // 0 == rem
        ctx->ctr += n_blocks_skip * (RNG_OUTPUTLEN / AES128_BLOCKSIZE);
        ctx->used = RNG_OUTPUTLEN;
    }
}

