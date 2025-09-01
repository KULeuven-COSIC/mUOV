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

#include "parallel_matrix_op_masked.h"


// extern uint8_t ClearXOR(uint8_t rand1);
// extern uint8_t ClearMultXOR(uint8_t rand1, uint8_t rand2);
// extern void ClearStack(size_t len);

void secDotProd_gf256(uint8_t z[N_SHARES], uint8_t x[UOV_L][N_SHARES], uint8_t y[UOV_L][N_SHARES]) {
    
    uint8_t u[N_SHARES][N_SHARES];
    uint8_t w[N_SHARES];
    uint8_t random;

    for (uint8_t j = 0; j < N_SHARES; j++) {
        for (uint8_t i = 0; i < N_SHARES; i++) {
            u[j][i] = 0;
        }
    }
    for (uint8_t i = 0; i < N_SHARES; i++) {
            w[i] = 0;
    }

    /* Compute and sum 𝑙 cross-products */ 
    for (uint8_t k = 0; k < UOV_L; k++) {
        for (uint8_t i = 0; i < N_SHARES; i++) {
            for (uint8_t j = i+1; j < N_SHARES; j++) {

                u[i][j] ^= gf256_mul(x[k][i], y[k][j]);
                u[j][i] ^= gf256_mul(x[k][j], y[k][i]);
            }
        }
        for (uint8_t i = 0; i < N_SHARES; i++) {
            w[i] ^= gf256_mul(x[k][i], y[k][i]);
        }
    }

    /* Resharing */
    for (uint8_t i = 0; i < N_SHARES; i++) {
        for (uint8_t j = i+1; j < N_SHARES; j++) {
            random = random_uint8();
            u[i][j] ^= random;
            u[j][i] ^= random;
        }
    }

    /* Compression */
    for (uint8_t i = 0; i < N_SHARES; i++) {
        z[i] = w[i];
        for (uint8_t j = 0; j < N_SHARES; j++) {
            if(j!=i) {
                z[i] ^= u[i][j];
            }
        }
    }
}

void secMatVec_gf256(uint8_t b[UOV_M][N_SHARES], uint8_t A[UOV_M][UOV_L][N_SHARES], uint8_t x[UOV_L][N_SHARES]) {
    for (uint8_t j = 0; j < UOV_M; j++) {
        secDotProd_gf256(b[j], A[j], x);
    }
}

void secMatVec_stop_early_gf256(uint8_t b[UOV_M][N_SHARES], uint8_t A[UOV_M][UOV_L][N_SHARES], uint8_t x[UOV_L][N_SHARES], uint8_t stop)
{
    for (uint8_t j = 0; j < UOV_M; j++)
    {
        secDotProd_gf256(b[j], A[j], x);
        if (j == stop)
        {
            break;
        }
    }
}

void secQuad_gf256(uint8_t y[UOV_M][N_SHARES], uint8_t P[UOV_M][UOV_L][UOV_L], uint8_t x[UOV_L][N_SHARES]) {

    uint8_t s[UOV_L][N_SHARES];
    uint8_t T[UOV_L][UOV_M][N_SHARES];
    uint8_t Ttrans[UOV_M][UOV_L][N_SHARES];
    
    for (uint8_t k = 0; k < UOV_L; k++){
        for (uint8_t i = 0; i < N_SHARES; i++) {
            s[k][i] = 0;
        }
        strongrefresh_gf256(s[k], x[k]);
    }

    
    for (uint8_t k = 0; k < UOV_L; k++){
        for (uint8_t j = 0; j < UOV_M; j++){
            for (uint8_t i = 0; i < N_SHARES; i++){
                T[k][j][i] = 0;
            }
        }
    }

    for (uint8_t j = 0; j < UOV_M; j++){
        for (uint8_t k = 0; k < UOV_L; k++){
            for (uint8_t w = 0; w < UOV_L; w++){
                for (uint8_t i = 0; i < N_SHARES; i++){
                    T[k][j][i] ^= gf256_mul(P[j][k][w], x[w][i]);
                }
            }
        }
    }
    for (uint8_t j = 0; j < UOV_M; j++){
        for (uint8_t k = 0; k < UOV_L; k++){
            for (uint8_t i = 0; i < N_SHARES; i++){
                Ttrans[j][k][i] = T[k][j][i];
            }
        }
    }

    secMatVec_gf256(y, Ttrans, s);
}
