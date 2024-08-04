/*
Sphynx Operating System

File: string.cpp
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

Description: POSIX C string functions
*/

#include <string.hpp>

extern "C" void* memset(void* d, int c, size_t n) {
    unsigned char* p = static_cast<unsigned char*>(d);
    while (n--) {
        *p++ = static_cast<unsigned char>(c);
    }
    return d;
}

extern "C" void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* p1 = static_cast<unsigned char*>(dest);
    const unsigned char* p2 = static_cast<const unsigned char*>(src);
    while (n--) {
        *p1++ = *p2++;
    }
    return dest;
}
