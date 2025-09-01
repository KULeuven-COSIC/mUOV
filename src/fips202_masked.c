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

#include "UOV_params_masking.h"
#include "fips202_masked.h"
#include "utils_prng.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define NROUNDS 24
#define ROL(a, offset) ((a << offset) ^ (a >> (64-offset)))

const int keccakf_piln[24] = {
    10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4,
    15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1};

const int keccakf_rotc[24] = {
    1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14,
    27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44};


static void __attribute__((noinline)) sha3_chi_masked(uint64_t state[25][N_SHARES], uint64_t bc[5][N_SHARES])
{
    int i, j, k, l;
    int64_t rand_no=N_SHARES*(N_SHARES-1)/2;
    uint64_t random[rand_no];
    uint8_t rand[8];
    
    for (i = 0; i < rand_no; i++){
    	randombytes(rand,8);
        memcpy(random+i, rand, sizeof(rand));
    }
    	
    for (j = 0; j < 25; j += 5) {
        for (i = 0; i < 5; i++)
        {
            for (k = 0; k < N_SHARES; k++)
            {
                bc[i][k] = state[j + i][k];
            }
        }
        for (i = 0; i < 5; i++)
        {
             for (l = 0; l < N_SHARES; l++)
             {
                state[j + i][l] ^= ((~bc[(i + 1) % 5][l]) & bc[(i + 2) % 5][l]);
                for (k = 0; k < N_SHARES; k++)
                {   
                    if(k<l) 
                        state[j+i][l] ^= (bc[(i + 1) % 5][l] & bc[(i + 2) % 5][k])^random[k+((l*(l-1))/2)];
                    else if(k>l)
                        state[j+i][l] ^= (bc[(i + 1) % 5][l] & bc[(i + 2) % 5][k])^random[l+((k*(k-1))/2)];
                }
	     }
      
        }
    }
}


static void __attribute__((noinline)) sha3_theta_rho_pi(uint64_t st[25][N_SHARES], uint64_t bc[5][N_SHARES])
{

    int i, j, k;
    uint64_t t;

    for (k = 0; k < N_SHARES; k++)
    {
        for (i = 0; i < 5; i++) {
            bc[i][k] = st[i][k] ^ st[i + 5][k] ^ st[i + 10][k] ^ st[i + 15][k] ^ st[i + 20][k];
        }

        for (i = 0; i < 5; i++) {
            t = bc[(i + 4) % 5][k] ^ ROL(bc[(i + 1) % 5][k], 1);
            for (j = 0; j < 25; j += 5) {
                st[j + i][k] ^= t;
            }
        }

        t = st[1][k];
        for (i = 0; i < 24; i++) {
            j = keccakf_piln[i];
            bc[0][k] = st[j][k];
            st[j][k] = ROL(t, keccakf_rotc[i]);
            t = bc[0][k];
        }
    }
}

static void __attribute__ ((noinline)) KeccakF1600_StatePermute_masked(uint64_t state[25][N_SHARES]) {
    int round;
    uint64_t bc[5][N_SHARES];

    // constants
    const uint64_t keccakf_rndc[24] = {
        0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
        0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
        0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008
    };

    for (round = 0; round < NROUNDS; round++) {

        // Theta Rho Pi
        // Either ClearStack() or explicitely pass bc1, bc2 for memory-reuse leakage
        sha3_theta_rho_pi(state, bc);

        //  Chi
        sha3_chi_masked(state, bc);

        //  Iota
        state[0][0] ^= keccakf_rndc[round];
    }
}

static void keccak_init_masked(uint64_t s[25][N_SHARES]) {
    unsigned int i;
    for (i = 0; i < 25; i++) {
        for (uint8_t j = 0; j < N_SHARES; j++) {
            s[i][j] = 0;
        }
    }
}

static unsigned int keccak_absorb_masked(uint64_t s[25][N_SHARES],
                                  unsigned int pos,
                                  unsigned int r,
                                  const uint8_t *in,
                                  size_t inlen) {
    unsigned int i;
    // uint32_t *original_in  = in;
    while (pos + inlen >= r) {
        for (i = pos; i < r; i++) {
            for (uint8_t j = 0; j < N_SHARES; j++) {
                s[i / 8][j] ^= (uint64_t) * in++ << 8 * (i % 8); // assumes row-major order
            }
            // s[i / 8] ^= (uint64_t) * in++ << 8 * (i % 8);
        }
        inlen -= r - pos;
        KeccakF1600_StatePermute_masked(s);
        pos = 0;
    }
    // in = original_in;
    for (i = pos; i < pos + inlen; i++) {
        for (uint8_t j = 0; j < N_SHARES; j++) {
            s[i / 8][j] ^= (uint64_t) * in++ << 8 * (i % 8);
        }
        // s[i / 8] ^= (uint64_t) * in++ << 8 * (i % 8);
    }

    return i;
}

static void keccak_squeezeblocks_masked(uint8_t *out,
                                 size_t nblocks,
                                 uint64_t s[25][N_SHARES],
                                 unsigned int r) {
    unsigned int i;

    while (nblocks) {
        KeccakF1600_StatePermute_masked(s);
        for (i = 0; i < r / 8; i++) {
            for (uint8_t j = 0; j < 8; j++) {
                for (uint8_t k = 0; k < N_SHARES; k++) {
                    // out[j + 8 * i + k] = s[i][k] >> 8 * j;
                    out[(j + 8 * i)*N_SHARES + k] = s[i][k] >> 8 * j;
                }
                // out[j] = u >> 8 * i;
            }
            // store64(out + 8 * i, s[i]);
        }
        out += r*N_SHARES;
        nblocks -= 1;
    }
}

static void keccak_finalize_masked(uint64_t s[25][N_SHARES], unsigned int pos, unsigned int r, uint8_t p) {
    s[pos / 8][0] ^= (uint64_t)p << 8 * (pos % 8);
    s[r / 8 - 1][0] ^= 1ULL << 63;
}

static unsigned int keccak_squeeze_masked(uint8_t *out,
                                   size_t outlen,
                                   uint64_t s[25][N_SHARES],
                                   unsigned int pos,
                                   unsigned int r) {
    unsigned int i;

    while (outlen) {
        if (pos == r) {
            KeccakF1600_StatePermute_masked(s);
            pos = 0;
        }
        for (i = pos; i < r && i < pos + outlen; i++) {
            for (uint8_t j = 0; j < N_SHARES; j++) {
                *out++ = s[i / 8][j] >> 8 * (i % 8);
                // printf("\n out = %d\n",s[i / 8][j] >> 8 * (i % 8));
            }
            // *out++ = s[i / 8] >> 8 * (i % 8);
        }
        // printf("\npos = %d\n",pos);
        outlen -= i - pos;
        pos = i;
    }

    return pos;
}


void shake256_init_masked(keccak_state_masked *state) {
    keccak_init_masked(state->s);
    state->pos = 0;
}

void shake256_absorb_masked(keccak_state_masked *state, const uint8_t *in, size_t inlen) {
    state->pos = keccak_absorb_masked(state->s, state->pos, SHAKE256_RATE, in, inlen);
}

void shake256_finalize_masked(keccak_state_masked *state) {
    keccak_finalize_masked(state->s, state->pos, SHAKE256_RATE, 0x1F);
    state->pos = SHAKE256_RATE;
}

void shake256_squeezeblocks_masked(uint8_t *out, size_t nblocks, keccak_state_masked *state) {
    keccak_squeezeblocks_masked(out, nblocks, state->s, SHAKE256_RATE);
}

void shake256_squeeze_masked(uint8_t *out, size_t outlen, keccak_state_masked *state) {
    state->pos = keccak_squeeze_masked(out, outlen, state->s, state->pos, SHAKE256_RATE);
}
