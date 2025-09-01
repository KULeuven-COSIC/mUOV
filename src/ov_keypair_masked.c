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

#include "ov_keypair_masked.h"
#include "utils_prng.h"
#include "blas_matrix_masked.h"
#include "refresh.h"
#include "parallel_matrix_op_masked.h"
#include "utils_hash_masked.h"

int generate_keypair_pkc_masked(cpk_masked_t *pk, sk_masked_t *sk, sk_seed_t *sk_seed) {
    
    unsigned char pk_seed[UOV_PKSEED_BYTE];
    unsigned char buf[(UOV_PKSEED_BYTE*N_SHARES) + sizeof(sk->O)];
    unsigned char *pk_seed_temp = buf;
    unsigned char *O = buf + (UOV_PKSEED_BYTE*N_SHARES);
    unsigned char O_T[UOV_M][UOV_L][N_SHARES];
    
    memcpy(sk->sk_seed, sk_seed, UOV_SKSEED_BYTE*N_SHARES); // assumes row-major order

    // Expand_sk: prng for sk
    mhash_ctx mhctx;
    hash_init_masked(&mhctx);
    hash_update_masked(&mhctx, (uint8_t *) sk_seed, UOV_SKSEED_BYTE);
    hash_final_digest_masked(buf, (UOV_O_matrix_BYTE+UOV_PKSEED_BYTE), &mhctx);

    for (int i = 0; i < UOV_PKSEED_BYTE; i++)
    {
        pk_seed[i] = pk_seed_temp[i*N_SHARES];
        for (int j = 1; j < N_SHARES; j++)
        {
            pk_seed[i] ^= pk_seed_temp[(i*N_SHARES)+j];
        }
    }
    
    memcpy(O_T, O, sizeof(O_T));

    for (int i = 0; i < UOV_L; i++)
    {
        for (int j = 0; j < UOV_M; j++)
        {
            for (int k = 0; k < N_SHARES; k++)
            {
                sk->O[i][j][k] = O_T[j][i][k];
            }
        }
    }
    
    memcpy( pk->pk_seed, pk_seed, UOV_PKSEED_BYTE );

    // Expand_P: prng for pk
    prng_publicinputs_t prng1; 
    unsigned char temp_P2[UOV_PK_P2_BYTE];
    unsigned char interm_P2[UOV_M][UOV_L][UOV_M];
    unsigned char P2[UOV_PK_P2_BYTE];

    prng_set_publicinputs(&prng1, pk_seed);
    // P1
    prng_gen_publicinputs(&prng1, sk->P1, sizeof(sk->P1));
    // P2
    prng_gen_publicinputs(&prng1, temp_P2, UOV_PK_P2_BYTE); // &P2 sk->S

    prng_release_publicinputs(&prng1);


    //P2 change
    for (uint8_t l = 0; l < UOV_L; l++){
        for (uint8_t m = 0; m < UOV_M; m++){
            for (uint8_t k = 0; k < UOV_M; k++){
                interm_P2[k][l][m] = temp_P2[((l*UOV_M*UOV_M)+(m*UOV_M)+k)];
            }
        }
    }

    memcpy( P2, interm_P2, UOV_PK_P2_BYTE );

    // Compute P3

    // Transpose + refresh O
    uint8_t Q[UOV_M][UOV_L][N_SHARES];
    for (uint8_t m = 0; m < UOV_M; m++)
    {
        for (uint8_t l = 0; l < UOV_L; l++)
        {
            strongrefresh_gf256(Q[m][l], sk->O[l][m]);
        }
    }
    
    // uint16_t row_offset;
    uint8_t A_tr[UOV_M][UOV_L][N_SHARES];
    uint8_t skP1[UOV_M][UOV_N_TRIANGLE_TERMS(UOV_L)];

    for (int l = 0; l < UOV_N_TRIANGLE_TERMS(UOV_L); l++){
        for (int m = 0; m < UOV_M; m++){
            skP1[m][l] = sk->P1[(l*UOV_M)+m];
        }
    }

    uint8_t P3_mat[UOV_M][UOV_M][UOV_M][N_SHARES]={0};
    uint8_t P3_mat_trans[UOV_M][UOV_M][UOV_M][N_SHARES]={0};


    for (uint8_t j = 0; j < UOV_M; j++)
    {
        // Calculate A

        mat_mul_add_gf256(sk->S[j], P2 + (j * UOV_L * UOV_M), sk->O, skP1[j]);


        // A^T
        for (uint8_t l = 0; l < UOV_L; l++)
        {
            for (uint8_t m = 0; m < UOV_M; m++)
            {
                for (uint8_t n = 0; n < N_SHARES; n++)
                {
                    A_tr[m][l][n] = sk->S[j][l][m][n];
                }
            }
        }

        // Calculate P3

        for (uint8_t k = 0; k < UOV_M; k++)
        {

            secMatVec_gf256(P3_mat_trans[j][k], A_tr, Q[k]);

        }

        
        for (int i1 = 0; i1 < UOV_M; i1++)
        {
            for (int i2 = 0; i2 < UOV_M; i2++)
            {
                for (int i3 = 0; i3 < N_SHARES; i3++)
                {
                    P3_mat[j][i1][i2][i3] = P3_mat_trans[j][i2][i1][i3];
                }
            }
        }


        // ExpandSK S[j]

        mat_mul_add_tr_gf256(sk->S[j], P2 + (j * UOV_L * UOV_M), sk->O, skP1[j]);

    }

    for (int j = 0; j < UOV_M; j++)
    {
        for (int i1 = 0; i1 < UOV_M; i1++)
        {
            for (int i2 = i1+1; i2 < UOV_M; i2++)
            {
                for (int i3 = 0; i3 < N_SHARES; i3++)
                {
                    P3_mat[j][i1][i2][i3] ^= P3_mat[j][i2][i1][i3];
                }
            }
        }
    }

    uint16_t P3_offset = 0;

    for (int i1 = 0; i1 < UOV_M; i1++)
    {
        for (int i2 = i1; i2 < UOV_M; i2++)
        {
            for (int j = 0; j < UOV_M; j++)
            {
                FullAdd_gf256(pk->P3+(P3_offset), P3_mat[j][i1][i2]);
                P3_offset++;
            }
        }
    }
    
    return 0;
}
