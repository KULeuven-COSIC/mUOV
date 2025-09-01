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

#include "bool_util.h"

void secNOT_gf256(uint8_t y_out[N_SHARES], uint8_t x_in[N_SHARES]) {
	y_out[0] = (1^x_in[0]);

    for (uint8_t i = 1; i < N_SHARES; i++) {
        y_out[i] = x_in[i];
    }
}

void secAND_gf256(uint8_t z_out[N_SHARES], uint8_t x_in[N_SHARES], uint8_t y_in[N_SHARES]) {
	uint8_t r[N_SHARES][N_SHARES];
	uint8_t random;

	for (uint8_t i = 0; i < N_SHARES; i++)
	{
		for (uint8_t j = (i + 1); j < N_SHARES; j++)
		{            		
			random = random_uint8();
			r[i][j] = random;
			r[j][i] = r[i][j] ^ (x_in[i] & y_in[j]);
			r[j][i] = r[j][i] ^ (x_in[j] & y_in[i]);
		}
	}

	for (uint8_t i = 0; i < N_SHARES; i++)
	{
		z_out[i] = x_in[i] & y_in[i];
		for (uint8_t j = 0; j < N_SHARES; j++)
		{
			if (i != j)
			{
				z_out[i] ^= r[i][j];
			}
		}
	}
}

void secOR_gf256(uint8_t z_out[N_SHARES], uint8_t x_in[N_SHARES], uint8_t y_in[N_SHARES]) {
    uint8_t x_inv[N_SHARES];
    uint8_t y_inv[N_SHARES];
    uint8_t z_inv[N_SHARES];
	#ifdef M4_TARGET
	uint8_t rand1;
	uint8_t rand_out;
	#endif

    secNOT_gf256(x_inv, x_in);
	#ifdef M4_TARGET
	rand1 = random_uint8();
	rand_out = ClearXOR(rand1); // clear barrell shifter 	
	#endif
    secNOT_gf256(y_inv, y_in);
    secAND_gf256(z_inv, x_inv, y_inv);
    secNOT_gf256(z_out, z_inv);
}
