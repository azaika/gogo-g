/*
Copyright (c) 2014-2017 Melissa O'Neill and PCG Project contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// https://github.com/rkern/pcg64 を必要な部分だけ拝借 & 改変

#ifndef GOGO_HEADER_RANDOM_H
#define GOGO_HEADER_RANDOM_H

#include <stdint.h>
#include <time.h>

typedef struct {
    __uint128_t state;
    __uint128_t inc;
} pcg64_state;

static uint64_t pcg_rotr_64(uint64_t value, unsigned int rot) {
    return (value >> rot) | (value << ((- rot) & 63));
}

static void pcg_128_step_r(pcg64_state* rng) {
    rng->state = rng->state * (((__uint128_t)2549297995355413924ULL << 64) + (__uint128_t)4865540595714422341ULL) + rng->inc;
}

static uint64_t pcg_output_xsl_rr_128_64(__uint128_t state) {
    return pcg_rotr_64(((uint64_t)(state >> 64u)) ^ (uint64_t)state, state >> 122u);
}

__uint128_t pcg_advance_lcg_128(__uint128_t state, __uint128_t delta, __uint128_t cur_mult, __uint128_t cur_plus) {
   __uint128_t acc_mult = 1u;
   __uint128_t acc_plus = 0u;

   while (delta > 0) {
       if (delta & 1) {
	   acc_mult *= cur_mult;
	   acc_plus = acc_plus * cur_mult + cur_plus;
       }
       cur_plus = (cur_mult + 1) * cur_plus;
       cur_mult *= cur_mult;
       delta /= 2;
   }
   return acc_mult * state + acc_plus;
}

static uint64_t pcg64_random(pcg64_state* rng) {
    pcg_128_step_r(rng);
    return pcg_output_xsl_rr_128_64(rng->state);
}

static void pcg64_srandom(pcg64_state* rng, __uint128_t initstate, __uint128_t initseq) {
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    pcg_128_step_r(rng);
    rng->state += initstate;
    pcg_128_step_r(rng);
}

static void pcg64_srandom_by_time(pcg64_state* rng) {
    __uint128_t a[8];
    for (int i = 0; i < 8; ++i) {
        a[i] = (__uint128_t)time(NULL) << (i % 4 * 32);
    }

    pcg64_srandom(rng, a[0] + a[1] + a[2] + a[3], a[4] + a[5] + a[6] + a[7]);
}

#endif // GOGO_HEADER_RANDOM_H