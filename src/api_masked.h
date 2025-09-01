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

#ifndef API_H
#define API_H

#include "UOV_params.h"
#include <stdint.h>

#define CRYPTO_SECRETKEYBYTES_masked UOV_SECRETKEYBYTES
#define CRYPTO_PUBLICKEYBYTES_masked UOV_PUBLICKEYBYTES
#define CRYPTO_BYTES_masked UOV_SIGNATUREBYTES
#define CRYPTO_ALGNAME_masked "UOV-Ip-pkc-masked"

int crypto_sign_keypair_masked(unsigned char *pk, unsigned char *sk, unsigned char *sk_seed);
int crypto_sign_signature_masked(unsigned char *sig, unsigned long long *siglen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk);
int crypto_sign_masked(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk);

 
#endif
