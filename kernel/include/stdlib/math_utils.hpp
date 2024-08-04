/*
Sphynx Operating System

File: math_utils.hpp
Author: Kevin Alavik
Year: 2024

License: MIT License

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

Description: Math related utility functions
*/

#pragma once

#include <common.hpp>
#include <stdint.h>

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

#define DIV_ROUNDUP(x, y) (((x) + (y) - 1) / (y))
#define DIV_ROUNDDOWN(x, y) (x / y)

#define BITMAP_SET_BIT(bitmap, bit_index) ((bitmap)[(bit_index) / 8] |= (1 << ((bit_index) % 8)))
#define BITMAP_UNSET_BIT(bitmap, bit_index) ((bitmap)[(bit_index) / 8] &= ~(1 << ((bit_index) % 8)))
#define BITMAP_READ_BIT(bitmap, bit_index) ((bitmap)[(bit_index) / 8] & (1 << ((bit_index) % 8)))
#define BITMAP_CLEAR(bitmap, size) do { \
    for (size_t i = 0; i < size; i++) { \
        (bitmap)[i] = 0; \
    } \
} while (0)

#define ALIGN_UP(x, base) (((x) + (base) - 1) & ~((base) - 1))
#define ALIGN_DOWN(x, base) ((x) & ~((base) - 1))

#define POW(base, exponent) ({ \
    uint64_t out = 1; \
    for (uint64_t i = 0; i < exponent; i++) { \
        out *= base; \
    } \
    out; \
})
