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

#include "ov_masked.h"

int ov_sign_masked( uint8_t *signature, const sk_masked_t *sk, const uint8_t *message, uint32_t mlen ) {
    // allocate temporary storage.
    uint8_t salt[UOV_SALT_BYTE];
    uint8_t t[UOV_N];
    uint8_t buf[UOV_L*N_SHARES];
    uint8_t vinegar[UOV_L][N_SHARES];
    uint8_t mat_l1[UOV_M][UOV_M][N_SHARES];
    uint8_t y[UOV_M][N_SHARES];
    uint8_t x[UOV_M];
    uint8_t u[UOV_L][N_SHARES];
    uint8_t w[UOV_L];
    uint8_t s[UOV_N];

    uint8_t fail=1;

    // randombytes(salt, UOV_SALT_BYTE);
    for (size_t i = 0; i < UOV_SALT_BYTE; i++)
    {
        salt[i] = i+1;
    }
    

    hash_ctx h_m_salt_secret;
    mhash_ctx h_vinegar_copy;
    // The computation:  H(M||salt)  -->   y  --C-map-->   x   --T-->   w
    hash_init  (&h_m_salt_secret);
    hash_update(&h_m_salt_secret, message, mlen);
    hash_update(&h_m_salt_secret, salt, UOV_SALT_BYTE);

    for (int i = 0; i < 25; i++){
        h_vinegar_copy.s[i][0] = h_m_salt_secret.s[i];
        for (int n = 1; n < N_SHARES; n++){
            h_vinegar_copy.s[i][n] = 0;
        }
    }
    h_vinegar_copy.pos = h_m_salt_secret.pos;

    hash_final_digest(t, UOV_M, &h_m_salt_secret);    // H(M||salt)

    hash_update_masked(&h_vinegar_copy, (uint8_t *) sk->sk_seed, UOV_SKSEED_BYTE);

    unsigned n_attempt = 0;
    while ( 256 > n_attempt ) {
        uint8_t ctr[N_SHARES];                    // counter for generating vinegar
        ctr[0] = n_attempt & 0xff;
        for (int n = 1; n < N_SHARES; n++)
            ctr[n] = 0;
        n_attempt++;

        mhash_ctx h_vinegar;
        hash_ctx_copy_masked(&h_vinegar, &h_vinegar_copy);
        hash_update_masked(&h_vinegar, (uint8_t *) ctr, 1 );                  // H(M||salt||sk_seed||ctr ...
        hash_final_digest_masked(buf, UOV_L, &h_vinegar);   // H(M||salt||sk_seed||ctr)
        memcpy(vinegar, buf, sizeof(buf));
        
        // generate linear system:
        // matrix
        uint8_t S_tr[UOV_M][UOV_L][N_SHARES];
        for (uint8_t j = 0; j < UOV_M; j++)
        {
            //S^T
            for (uint8_t l = 0; l < UOV_L; l++){
                for (uint8_t m = 0; m < UOV_M; m++){
                    for (uint8_t n = 0; n < N_SHARES; n++){
                        S_tr[m][l][n] = sk->S[j][l][m][n];
                    }
                }
            }
            secMatVec_gf256(mat_l1[j], S_tr, vinegar);
        }

        uint8_t skP1[UOV_M][UOV_N_TRIANGLE_TERMS(UOV_L)]; //extra need to remove latter
        for (int l = 0; l < UOV_N_TRIANGLE_TERMS(UOV_L); l++){
            for (int m = 0; m < UOV_M; m++){
                skP1[m][l] = sk->P1[(l*UOV_M)+m];
            }
        }

        uint8_t sk_P1[UOV_M][UOV_L][UOV_L]={0};
        int32_t offset;
        for (uint8_t m = 0; m < UOV_M; m++){
            offset = 0;
            for (uint8_t i = 0; i < UOV_L; i++){
                for (uint8_t j = i; j < UOV_L; j++){
                    sk_P1[m][i][j] = skP1[m][offset]; 
                    offset++;
                }
            }
        }

        secQuad_gf256(y, sk_P1, vinegar);

        for (uint8_t m = 0; m < UOV_M; m++){
            y[m][0] ^= t[m];      // substract the contribution from vinegar variables
        }

        unsigned l1_succ = secRowEch_gf256(mat_l1, y);
        if ( l1_succ != 0 ) {
            continue;}
        secBackSub_gf256(x, mat_l1, y);

        // while (fail == 1) {
        //     fail = secRowEch_gf256(mat_l1, y);
        // }

        // if (fail == 0) {
        //     secBackSub_gf256(x, mat_l1, y);     
        // }
        // else {
        //     continue;
        // }

        break;
    }

    if ( 255 <= n_attempt ) {
        return -1;}

    mat_mul_add_sign_gf256(u, vinegar, sk->O, x);

    for (uint8_t l = 0; l < UOV_L; l++){
        FullAdd_gf256(&w[l], u[l]);
    }

    // s <- w || x
    memcpy(s, w, UOV_L);
    memcpy(s + UOV_L, x, UOV_M);

    // return: signature <- s || salt.
    memcpy(signature, s, UOV_N);
    memcpy(signature + UOV_N, salt, UOV_SALT_BYTE);

    return 0;
}
