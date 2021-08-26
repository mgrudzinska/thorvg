/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef THORVG_AVX_VECTOR_SUPPORT

#include <immintrin.h>

#define N_32BITS_IN_256REG 8

static inline void avxRasterRGBA32(uint32_t *dst, uint32_t val, uint32_t offset, int32_t len)
{
    //1. set the beginning of the array
    dst += offset;

    //2. fill the not aligned memory (for a 256-bit register a 32-bytes alignment is required)
    uint32_t notAligned = ((uintptr_t)dst & 0x1f) / 4;
    if (notAligned) {
        notAligned = (N_32BITS_IN_256REG - notAligned > (uint32_t)len ? (uint32_t)len : N_32BITS_IN_256REG - notAligned);
        for (uint32_t x = 0; x < notAligned; ++x) *dst++ = val;
    }

    //3. calculate how many iterations we need to cover the length
    uint32_t iterations = (len - notAligned) / N_32BITS_IN_256REG;
    uint32_t avxFilled = iterations * N_32BITS_IN_256REG;

    //4. fill the octets
    __m256i avxVal = _mm256_set1_epi32(val);
    auto avxDst = (__m256i*)dst;
    for (uint32_t i = 0; i < iterations; ++i) {
        *avxDst = avxVal;
        avxDst++;
    }

    //4. fill leftovers (in the first step set the pointer to the place where the avx job is done)
    int32_t leftovers = len - notAligned - avxFilled;
    dst += avxFilled;
    while (leftovers--) *dst++ = val;
}

#endif
