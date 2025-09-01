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

#include "blas_matrix_masked.h"

int secRowEch_gf256(uint8_t A[UOV_M][UOV_M][N_SHARES], uint8_t b[UOV_M][N_SHARES]) {
    uint8_t p[N_SHARES];
    uint8_t s[N_SHARES];

    for (uint8_t j = 0; j < UOV_M; j++) {
        
        // STEP 1
        #ifdef UOV_EARLY_STOP
            uint8_t end = ((j + UOV_STOP) < UOV_M) ? j + UOV_STOP : UOV_M; 
        #else 
            uint8_t end = UOV_M;    
        #endif
        
        for (uint8_t k = j + 1; k < end; k++) {
            uint8_t z_temp[N_SHARES];
            uint8_t z[N_SHARES];

            secNonzero_gf256(z_temp, A[j][j]);
            secNOT_gf256(z, z_temp);

            for (uint8_t l = j; l < UOV_M; l++) {
                secCondAdd_gf256(A[j][l], A[j][l], A[k][l], z);
            }
            secCondAdd_gf256(b[j], b[j], b[k], z);
        }

        // STEP 2
        uint8_t t[N_SHARES];
        uint8_t cj;

        secNonzero_gf256(t, A[j][j]);
        FullAdd_gf256(&cj, t);

        if (cj == 0) {
            return -1; // Fail
        }
        else {
            // STEP 3
            B2Minv_gf256(p, A[j][j]);

            for (uint8_t k = j; k < UOV_M; k++) {
                secScalarMult_gf256(A[j][k], A[j][k], p);
            }
            secScalarMult_gf256(b[j], b[j], p);

            // STEP 4
            for (uint8_t k = j + 1; k < UOV_M; k++) {
                strongrefresh_gf256(s, A[k][j]);
                for (uint8_t l = j; l < UOV_M; l++) {
                    secMultSub_gf256(A[k][l], A[j][l], A[k][l], s);
                }
                secMultSub_gf256(b[k], b[j], b[k], s);
            }
        }
    } 

    return 0; // Success
}

void secBackSub_gf256(uint8_t x[UOV_M], uint8_t A[UOV_M][UOV_M][N_SHARES], uint8_t b[UOV_M][N_SHARES]) 
{
    for (int j = UOV_M - 1; j > 0; j--) {
        FullAdd_gf256(&x[j], b[j]);

        for (int k = 0; k < j; k++) {
            for (int l = 0; l < N_SHARES; l++) {
                b[k][l] ^= gf256_mul(x[j], A[k][j][l]);
            }
        }
    }
    FullAdd_gf256(&x[0], b[0]);
}

void mat_mul_add_gf256(uint8_t A[UOV_L][UOV_M][N_SHARES], uint8_t P2[UOV_L * UOV_M], uint8_t O[UOV_L][UOV_M][N_SHARES], uint8_t P1[UOV_N_TRIANGLE_TERMS(UOV_L)])
{
    uint16_t row_offset = 0;
    for (uint8_t l = 0; l < UOV_L; l++)
    {
        for (uint8_t m = 0; m < UOV_M; m++)
        {
            A[l][m][0] = P2[l*UOV_M+m];
            for (uint8_t n = 1; n < N_SHARES; n++)
                A[l][m][n] = 0;
        }
        for (uint8_t m = 0; m < UOV_M; m++)
        {
            for (uint8_t k = l; k < UOV_L; k++) // Skip 0 entries in triangular matrix P1
            {
                for (uint8_t n = 0; n < N_SHARES; n++)
                {
                    A[l][m][n] ^= gf256_mul(P1[(k - l) + row_offset], O[k][m][n]); // l,k  k,m

                }
            }    
        }
        row_offset += UOV_L - l;
    }  
}

void mat_mul_add_tr_gf256(uint8_t A[UOV_L][UOV_M][N_SHARES], uint8_t P2[UOV_L * UOV_M], uint8_t O[UOV_L][UOV_M][N_SHARES], uint8_t P1[UOV_N_TRIANGLE_TERMS(UOV_L)])
{
    uint8_t P1_mat[UOV_L][UOV_L]={0};
    uint8_t P1_mat_trans[UOV_L][UOV_L];

    int off_set;

    off_set=0;
    for (int i = 0; i < UOV_L; i++)
    {
        for (int j = i; j < UOV_L; j++)
        {
            P1_mat[i][j] = P1[off_set];
            off_set++;
        }
    }

    for (int i = 0; i < UOV_L; i++)
    {
        for (int j = 0; j < UOV_L; j++)
        {
            P1_mat_trans[i][j] = P1_mat[j][i];
        }
    }

    for (int i = 0; i < UOV_L; i++)
    {
        for (int j = 0; j < UOV_L; j++)
        {
            P1_mat[i][j] ^= P1_mat_trans[i][j];
        }
    }

    for (uint8_t k = 0; k < UOV_L; k++)
    {
        for (uint8_t l = 0; l < UOV_M; l++)
        {
            A[k][l][0] = P2[k*UOV_M+l];
            for (uint8_t m = 1; m < N_SHARES; m++)
            {
                A[k][l][m] = 0;
            }
        }
    }


    for (uint8_t l = 0; l < UOV_L; l++)
    {
        for (uint8_t m = 0; m < UOV_M; m++)
        {
            for (uint8_t k = 0; k < UOV_L; k++)
            {
                for (uint8_t n= 0; n < N_SHARES; n++)
                {
                    A[l][m][n] ^= gf256_mul(P1_mat[l][k], O[k][m][n]);
                }
            }
        }
    }
}

void mat_mul_add_sign_gf256(uint8_t u[UOV_L][N_SHARES], uint8_t vinegar[UOV_L][N_SHARES], const uint8_t O[UOV_L][UOV_M][N_SHARES], uint8_t x[UOV_M])
{
    for (uint8_t l = 0; l < UOV_L; l++){
        for (uint8_t n = 0; n < N_SHARES; n++){
            u[l][n] = vinegar[l][n];
        }
    }
    for (uint8_t l = 0; l < UOV_L; l++){
        for (uint8_t m = 0; m < UOV_M; m++){
            for (uint8_t n = 0; n < N_SHARES; n++){
                u[l][n] ^= gf256_mul(O[l][m][n], x[m]);
            }
        }
    }
     
}