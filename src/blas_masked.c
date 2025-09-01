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

#include "blas_masked.h"

void secCondAdd_gf256(uint8_t s_out[N_SHARES], uint8_t x_in[N_SHARES], uint8_t y_in[N_SHARES], uint8_t b_in[N_SHARES]) {
    uint8_t a[N_SHARES];
    uint8_t b_ext[N_SHARES];
    uint8_t s_temp[N_SHARES];
    #ifdef M4_TARGET
    uint8_t rand1, rand_out;

    rand1 = random_uint8();
    #endif

    for (uint8_t i = 0; i < N_SHARES; i++) {
        #ifdef M4_TARGET
        rand_out = ClearXOR(rand1);
        #endif
        b_ext[i] = (uint8_t)(-b_in[i]); // sign extend
    }
    
    #ifdef M4_TARGET
    rand_out = ClearAND(rand1);
    rand_out = ClearXOR(rand1);
    #endif
    secAND_gf256(a, y_in, b_ext);
    #ifdef M4_TARGET
    rand_out = ClearAND(rand1);
    #endif

    for (uint8_t i = 0; i < N_SHARES; i++) {
        #ifdef M4_TARGET
        rand_out = ClearXOR(rand1);
        #endif
        s_temp[i] = a[i] ^ x_in[i];
    }
    
    #ifdef M4_TARGET
    rand_out = ClearXOR(rand1);
    #endif
    strongrefresh_gf256(s_out, s_temp);
}

void secScalarMult_gf256(uint8_t y_out[N_SHARES], uint8_t x_in[N_SHARES], uint8_t p_in[N_SHARES]) {
    for (uint8_t i = 0; i < N_SHARES; i++) {
        y_out[i] = x_in[i];
    }

    #ifdef M4_TARGET
    uint8_t rand1, rand2, rand_out;
    uint16_t random;
    random = random_uint16();
    rand1 = random&0xff;
    rand2 = (random>>8);
    rand_out = ClearMultXOR(rand1, rand2);
    #endif

    for (uint8_t j = 0; j < N_SHARES; j++) {
        for (uint8_t i = 0; i < N_SHARES; i++) {
            y_out[i] = gf256_mul(y_out[i], p_in[j]); 
        }
        refresh_gf256(y_out, y_out);
    }
}

void secMultSub_gf256(uint8_t z_out[N_SHARES], uint8_t x_in[N_SHARES], uint8_t y_in[N_SHARES], uint8_t c_in[N_SHARES]) {
    uint8_t t[N_SHARES];
    
    secMult_gf256(t, x_in, c_in);

    for (uint8_t i = 0; i < N_SHARES; i++) {
        z_out[i] = y_in[i] ^ t[i];
    }
}
