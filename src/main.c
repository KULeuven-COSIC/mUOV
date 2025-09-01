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

#include "main.h"

int main(void)
{
    #ifdef M4_TARGET
    // Initialize board
    clock_setup();
    gpio_setup();
    usart_setup(115200);
    flash_setup();
    rng_enable();

    char buffer[15];
    #endif

    #ifdef CPU_TARGET
    printf("%s\n", CRYPTO_ALGNAME_masked );
    printf("sk size: %d\n", CRYPTO_SECRETKEYBYTES_masked );
    printf("pk size: %d\n", CRYPTO_PUBLICKEYBYTES_masked );
    printf("signature size: %d\n\n", CRYPTO_BYTES_masked );
    #endif

    #ifdef RUN_BENCHMARK

    #ifdef M4_TARGET
    SCS_DEMCR |= SCS_DEMCR_TRCENA;
    DWT_CYCCNT = 0;
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;

    unsigned int start_total_count, end_total_count; 
    #endif
    
    #ifdef CPU_TARGET
    struct benchmark bm1, bm2;
    bm_init(&bm1);
    bm_init(&bm2);
    
    char msg[256];
    for (unsigned i = 0; i < 256; i++) {
        msg[i] = i;
    }
    #endif

    #endif

    unsigned char sm_masked[256 + CRYPTO_BYTES_masked];
    unsigned char m_masked[256];
    for (unsigned i = 0; i < 256; i++) {
        m_masked[i] = i;
    }
    unsigned long long mlen_masked = 256;
    unsigned long long smlen_masked;

    #ifdef CPU_TARGET
    unsigned char *pk_masked = (unsigned char *)malloc( CRYPTO_PUBLICKEYBYTES_masked );
    unsigned char *sk_masked = (unsigned char *)malloc( CRYPTO_SECRETKEYBYTES_masked );
    #endif
    #ifdef M4_TARGET
    unsigned char pk_masked[CRYPTO_PUBLICKEYBYTES_masked];
    unsigned char sk_masked[CRYPTO_SECRETKEYBYTES_masked];
    #endif
    
    uint8_t sk_masked_seed[UOV_SKSEED_BYTE*N_SHARES];
    uint8_t sk_seed_cc[UOV_SKSEED_BYTE];

    #ifdef CPU_TARGET
    printf("===========  START crypto_sign_keypair_masked()  ================\n\n");
    #endif

    for (uint32_t n_iter = 0; n_iter < N_RUNS; n_iter++)
    {
        // Initialize inputs
        for (int i1 = 0; i1 < UOV_SKSEED_BYTE; i1++)
        {
            sk_seed_cc[i1] = (n_iter+1)*i1;
            randombytes( sk_masked_seed+(N_SHARES*i1), (N_SHARES-1));
            sk_masked_seed[(N_SHARES*i1)+N_SHARES-1] = sk_seed_cc[i1];
            for (int i2 = 0; i2 < (N_SHARES-1); i2++)
                sk_masked_seed[(N_SHARES*i1)+N_SHARES-1] ^= sk_masked_seed[(N_SHARES*i1)+i2];
        }   
        for (unsigned j = 3; j < 256; j++) {
            m_masked[j] = (n_iter * j) & 0xff;
        } 

        #ifdef M4_TARGET
        sprintf(buffer, "iteration: %d", (int) n_iter);
        send_USART_str(buffer);
        #endif

        //////////////////////////
        // Start mCompactKeyGen //
        //////////////////////////

        int mr;
        int mrs;
        
        #ifdef RUN_BENCHMARK

        #ifdef M4_TARGET
        DWT_CYCCNT = 0;
        start_total_count = DWT_CYCCNT; 

        mr = crypto_sign_keypair_masked( pk_masked, sk_masked, sk_masked_seed);
        #endif

        #ifdef CPU_TARGET
        BENCHMARK(bm1, {
            mr = crypto_sign_keypair_masked( pk_masked, sk_masked, sk_masked_seed);
        });
        #endif

        #ifdef M4_TARGET
        end_total_count = DWT_CYCCNT - start_total_count;
        DWT_CYCCNT = 0;
        sprintf(buffer, "total cyc: %d", end_total_count);
        send_USART_str(buffer);
        #endif

        #endif

        #ifndef RUN_BENCHMARK
        mr = crypto_sign_keypair_masked( pk_masked, sk_masked, sk_masked_seed);
        #endif

        if ( 0 != mr ) {
            #ifdef CPU_TARGET
            printf("key generation return %d.\n", mr);
            #endif
            return -1;
        }

        /////////////////
        // Start mSign //
        /////////////////

        #ifdef RUN_BENCHMARK

        #ifdef M4_TARGET
        DWT_CYCCNT = 0;
        start_total_count = DWT_CYCCNT; 

        mrs = crypto_sign_masked( sm_masked, &smlen_masked, m_masked, mlen_masked, sk_masked );
        #endif

        #ifdef CPU_TARGET
        BENCHMARK(bm2, {
            mrs = crypto_sign_masked( sm_masked, &smlen_masked, m_masked, mlen_masked, sk_masked );
        });
        #endif

        #ifdef M4_TARGET
        end_total_count = DWT_CYCCNT - start_total_count;
        DWT_CYCCNT = 0;
        sprintf(buffer, "total cyc: %d", end_total_count);
        send_USART_str(buffer);
        #endif

        #endif

        #ifndef RUN_BENCHMARK
        mrs = crypto_sign_masked( sm_masked, &smlen_masked, m_masked, mlen_masked, sk_masked );
        #endif

        if ( 0 != mrs ) {
            #ifdef CPU_TARGET
            printf("signature generation return %d.\n", mrs);
            #endif
            return -1;
        }
    }

    #ifdef CPU_TARGET
    #ifdef RUN_BENCHMARK
    bm_dump(msg, 256, &bm1);
    printf("benchmark:  crypto_sign_keypair_masked(): %s\n\n", msg );
    bm_dump(msg, 256, &bm2);
    printf("benchmark:  crypto_sign_masked(): %s\n\n", msg );
    #endif

    free( pk_masked );
    free( sk_masked );
    #endif

    return 0;
}
