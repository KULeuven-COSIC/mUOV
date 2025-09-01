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

#include "refresh.h"

void refresh_gf256(uint8_t y_out[N_SHARES], uint8_t x_in[N_SHARES]) {
	uint8_t random;

    y_out[0] = x_in[0];
    for (uint8_t i = 1; i < N_SHARES; i++) {
		random = random_uint8();
        y_out[0] ^= random; 
        y_out[i] = x_in[i] ^ random; 
    }
}

void strongrefresh_gf256(uint8_t y_out[N_SHARES], uint8_t x_in[N_SHARES]) {
	uint8_t random;

    for (uint8_t i = 0; i < N_SHARES; i++) {
        y_out[i] = x_in[i];
    }
    for (uint8_t i = 0; i < N_SHARES; i++) {
        for (uint8_t j = i + 1; j < N_SHARES; j++) {
			random = random_uint8();
            y_out[i] ^= random; 
            y_out[j] ^= random; 
        }
    }
}

void FullAdd_gf256(uint8_t* y_out, uint8_t x_in[N_SHARES]) {
    uint8_t x_ref[N_SHARES];

    strongrefresh_gf256(x_ref, x_in);

    *y_out = x_ref[0];
    for (uint8_t i = 1; i < N_SHARES; i++) {
        *y_out ^= x_ref[i];
    }
}
