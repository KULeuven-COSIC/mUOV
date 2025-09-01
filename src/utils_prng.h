#ifndef _UTILS_PRNG_H_
#define _UTILS_PRNG_H_

#include <stdint.h>

#include "UOV_params.h"  // for macro _4ROUND_AES_
#include "mask_util.h"   // declare randombytes(), including here for backward compatibility

//
// This is not for traditional PRNG functions. It is PRNG with PUBLIC INPUTS.
// It uses AES128CTR or CTR with only 4 rounds AES128.
//

#define AES128CTR_KEYLEN   16
#define AES128CTR_NONCELEN 16
#define AES128_BLOCKSIZE 16


/////////////////  defination of prng_publicinputs_t  /////////////////////////////////

// TODO: check that this is actually the best value here;
// for aes256ctr.c it needs to be 64, but for openssl, we may change it
#define RNG_OUTPUTLEN 64

typedef struct {
    unsigned used;
    uint32_t ctr;
    unsigned char   buf[RNG_OUTPUTLEN];

    #ifdef _4ROUND_AES_
    uint32_t key[40];
    #else  // round key of the normal aes128
    uint32_t key[88];
    #endif
} prng_publicinputs_t;

int prng_set_publicinputs(prng_publicinputs_t *ctx, const unsigned char prng_seed[16]);

int prng_gen_publicinputs(prng_publicinputs_t *ctx, unsigned char *out, unsigned long outlen);

void prng_skip_publicinputs(prng_publicinputs_t *ctx, unsigned long outlen);

void prng_release_publicinputs(prng_publicinputs_t *ctx);

#endif // _UTILS_PRNG_H_


