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

#include "api_masked.h"
#include "ov_keypair_masked.h"
#include "ov_masked.h"
#include "UOV_params.h"
#include <stdint.h>
#include <string.h>

volatile uint32_t randclear = 0;

int crypto_sign_keypair_masked(unsigned char *pk, unsigned char *sk, unsigned char *sk_seed) {

    int r = generate_keypair_pkc_masked((cpk_masked_t *) pk, (sk_masked_t *) sk, (sk_seed_t *) sk_seed);

    return r;
}

int crypto_sign_signature_masked(unsigned char *sig, unsigned long long *siglen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk) {
   
    int r = -1;

    r = ov_sign_masked( sig, (const sk_masked_t *)sk, m, mlen );

    siglen[0] = UOV_SIGNATUREBYTES;
    
    return r;
}

int crypto_sign_masked(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
{
    int r = crypto_sign_signature_masked(sm + mlen, smlen, m, mlen, sk);

    memmove( sm, m, mlen );
    smlen[0] = mlen + UOV_SIGNATUREBYTES;

    return r;
}