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

#include "B2M.h"

void B2M_gf256(uint8_t M_out[N_SHARES], uint8_t B_in[N_SHARES]) {
	uint8_t random, random_inner;
    uint8_t B_in_temp[N_SHARES];

    #ifdef M4_TARGET
    uint16_t random_clear;
    uint8_t rand1, rand2, random_out;

    random_clear = random_uint16();
    rand1 = random_clear&0xff;
    rand2 = (random_clear>>8);
    #endif

    for (uint8_t i = 0; i < N_SHARES; i++) {
        #ifdef M4_TARGET
        B_in_temp[i] = rand1;
        #endif
	    B_in_temp[i] = B_in[i];
	}

    #ifdef M4_TARGET
    M_out[0] = rand2;
    #endif
    M_out[0] = B_in_temp[0];
    for (uint8_t i = 1; i < N_SHARES; i++) {
        random = random_uint8();

        while (random==0)
            random = random_uint8();
        
        #ifdef M4_TARGET
        random_out = ClearMultXOR(rand1, rand2);
        #endif

        M_out[i] = random; 
        M_out[0] = gf256_mul(M_out[0], M_out[i]);
        
        for (uint8_t j = 1; j < (N_SHARES-i); j++) {
			random_inner = random_uint8();

            while (random_inner==0)
                random_inner = random_uint8();

            #ifdef M4_TARGET
            random_out = ClearMultXOR(rand1, rand2); 
            #endif   

            B_in_temp[j] = gf256_mul(M_out[i], B_in_temp[j]); 
            B_in_temp[j] ^= random_inner; 
            M_out[0] ^= B_in_temp[j];
            B_in_temp[j] = random_inner; 
        }
        
        #ifdef M4_TARGET
        random_out = ClearMultXOR(rand1, rand2);
        #endif

		B_in_temp[N_SHARES-i] = gf256_mul(B_in_temp[N_SHARES-i], M_out[i]);
		M_out[0] ^= B_in_temp[N_SHARES-i];
        M_out[i] = gf256_inv(M_out[i]); 
    }
    
}

void B2Minv_gf256(uint8_t Minv_out[N_SHARES], uint8_t B_in[N_SHARES]) {
    uint8_t M_res[N_SHARES];

    B2M_gf256(M_res, B_in);
    secInv_gf256(Minv_out, M_res);
}
