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

#include "gf256_util.h"

void secMult_gf256(uint8_t z_out[N_SHARES], uint8_t x_in[N_SHARES], uint8_t y_in[N_SHARES]) {
    uint8_t r[N_SHARES][N_SHARES];
    uint8_t rand1;
    #ifdef M4_TARGET
    uint16_t random;
    uint8_t rand2;
    uint8_t random_out;
    #endif

	for (uint8_t i = 0; i < N_SHARES; i++)
	{
		for (uint8_t j = (i + 1); j < N_SHARES; j++)
		{
            #ifdef CPU_TARGET
            rand1 = random_uint8();
            #endif 
            #ifdef M4_TARGET    
            random = random_uint16();
            rand1 = random&0xff;
            rand2 = (random>>8);
            #endif
			r[i][j] = rand1;
            uint8_t xiyj;
            uint8_t xjyi;
            xiyj = gf256_mul(x_in[i], y_in[j]);
            #ifdef M4_TARGET
            random_out = ClearMultXOR(rand1, rand2);
            #endif
            xjyi = gf256_mul(x_in[j], y_in[i]);
			r[j][i] = r[i][j] ^ xiyj;
			r[j][i] = r[j][i] ^ xjyi;
            #ifdef M4_TARGET
            random_out = ClearMultXOR(rand1, rand2);
            #endif
		}
	}

	for (uint8_t i = 0; i < N_SHARES; i++)
	{
		z_out[i] = gf256_mul(x_in[i], y_in[i]);
		for (uint8_t j = 0; j < N_SHARES; j++)
		{
			if (i != j)
			{
				z_out[i] ^= r[i][j];
			}
		}
        #ifdef M4_TARGET
        random_out = ClearMultXOR(rand1, rand2);
        #endif
	}
}

void secNonzero_gf256(uint8_t b[N_SHARES], uint8_t x[N_SHARES]) {
    uint8_t t[N_SHARES];
    uint8_t t2[N_SHARES];
    uint8_t l[N_SHARES];
    uint8_t l2[N_SHARES];
    uint8_t l_fresh[N_SHARES];
    uint8_t r[N_SHARES];
    uint8_t r2[N_SHARES];
    #ifdef M4_TARGET
    uint8_t random, random_out;
    #endif

    for (uint8_t i = 0; i < N_SHARES; i++) {
        t[i] = x[i];
    }

    #ifdef M4_TARGET
    random = random_uint8();
    #endif

    // ITER 1 8b -> 4b
    for (uint8_t i = 0; i < N_SHARES; i++) {
        #ifdef M4_TARGET
        random_out = ClearAND(random);
        random_out = ClearXOR(random);
        #endif
        l[i] = (t[i] & 0xf0) >> 4;
        #ifdef M4_TARGET
        random_out = ClearAND(random);
        random_out = ClearXOR(random);
        #endif
        r[i] = (t[i] & 0x0f);
    }
    #ifdef M4_TARGET
    random_out = ClearAND(random);
    random_out = ClearXOR(random);
    #endif
    strongrefresh_gf256(l_fresh, l);
    #ifdef M4_TARGET
    random_out = ClearAND(random);
    random_out = ClearXOR(random);
    #endif
    secOR_gf256(t2, l_fresh, r);
    
    // ITER 2 4b -> 2b
    for (uint8_t i = 0; i < N_SHARES; i++) {
        #ifdef M4_TARGET
        random_out = ClearAND(random);
        random_out = ClearXOR(random);
        #endif
        l2[i] = (t2[i] & 0x0c) >> 2;
        #ifdef M4_TARGET
        random_out = ClearAND(random);
        random_out = ClearXOR(random);
        #endif
        r2[i] = (t2[i] & 0x03);
    }
    #ifdef M4_TARGET
    random_out = ClearAND(random);
    random_out = ClearXOR(random);
    #endif
    strongrefresh_gf256(l2, l2);
    #ifdef M4_TARGET
    random_out = ClearAND(random);
    random_out = ClearXOR(random);
    #endif
    secOR_gf256(t, l2, r2);

    // ITER 3 2b -> 1b
    for (uint8_t i = 0; i < N_SHARES; i++) {
        #ifdef M4_TARGET
        random_out = ClearAND(random);
        random_out = ClearXOR(random);
        #endif
        l[i] = (t[i] & 0x02) >> 1;
        #ifdef M4_TARGET
        random_out = ClearAND(random);
        random_out = ClearXOR(random);
        #endif
        r[i] = (t[i] & 0x01);
    }
    #ifdef M4_TARGET
    random_out = ClearAND(random);
    random_out = ClearXOR(random);
    #endif
    strongrefresh_gf256(l, l);
    #ifdef M4_TARGET
    random_out = ClearAND(random);
    random_out = ClearXOR(random);
    #endif
    secOR_gf256(t, l, r);

    for (uint8_t i = 0; i < N_SHARES; i++) {
        #ifdef M4_TARGET
        random_out = ClearAND(random);
        #endif
        b[i] = (t[i] & 0x01); 
    }
}

void secInv_gf256(uint8_t x_inv[N_SHARES], uint8_t x[N_SHARES]) { 
	for (uint8_t i = 0; i < N_SHARES; i++) {
		x_inv[i] = gf256_inv(x[i]);
	}
}
