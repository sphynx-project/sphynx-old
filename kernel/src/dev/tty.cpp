/*
Sphynx Operating System

File: tty.cpp
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

Description: Sphynx TTY system
*/

#include <dev/tty.hpp>
#include <common.hpp>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW 1
typedef long ssize_t;

#define NANOPRINTF_IMPLEMENTATION
#include <external/nanoprintf.h>

#include <dev/serial.hpp>

void _putc(char ch) {
    flanterm_write(ftCtx, &ch, sizeof(ch));
}

void _dputc(char ch) {
    Serial::outb(0xE9, ch);
}

int kprintf(const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    
    va_start(args, fmt);
    
    int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    if (length < 0 || length >= (int)sizeof(buffer)) {
        return -1;
    }
    
    for (int i = 0; i < length; ++i) {
        _putc(buffer[i]);
    }
    
    return length;
}

int kdprintf(const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    
    va_start(args, fmt);
    
    int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    if (length < 0 || length >= (int)sizeof(buffer)) {
        return -1;
    }
    
    for (int i = 0; i < length; ++i) {
        _dputc(buffer[i]);
    }
    
    return length;
}