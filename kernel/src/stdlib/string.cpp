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

extern "C" void* memmove(void* dest, const void* src, size_t n) {
    unsigned char* p1 = static_cast<unsigned char*>(dest);
    const unsigned char* p2 = static_cast<const unsigned char*>(src);

    if (p1 < p2) {
        while (n--) {
            *p1++ = *p2++;
        }
    } else {
        p1 += n;
        p2 += n;
        while (n--) {
            *--p1 = *--p2;
        }
    }
    return dest;
}

extern "C" size_t strlen(const char* s) {
    const char* p = s;
    while (*p) {
        ++p;
    }
    return p - s;
}

extern "C" int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
    }
    return static_cast<unsigned char>(*s1) - static_cast<unsigned char>(*s2);
}

extern "C" char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

extern "C" char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) {
        ++d;
    }
    while ((*d++ = *src++));
    return dest;
}

extern "C" const char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == static_cast<char>(c)) {
            return s;
        }
        ++s;
    }
    return nullptr;
}

extern "C" const char* strrchr(const char* s, int c) {
    const char* last_occurrence = nullptr;
    while (*s) {
        if (*s == static_cast<char>(c)) {
            last_occurrence = s;
        }
        ++s;
    }
    return last_occurrence;
}

extern "C" const char* strstr(const char* haystack, const char* needle) {
    if (!*needle) {
        return haystack;
    }
    for (; *haystack; ++haystack) {
        if ((*haystack == *needle) && (strcmp(haystack, needle) == 0)) {
            return haystack;
        }
    }
    return nullptr;
}

extern "C" char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (n && (*d++ = *src++)) {
        --n;
    }
    if (n) {
        while (--n) {
            *d++ = '\0';
        }
    }
    return dest;
}

extern "C" int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
        --n;
    }
    return (n == 0) ? 0 : static_cast<unsigned char>(*s1) - static_cast<unsigned char>(*s2);
}
