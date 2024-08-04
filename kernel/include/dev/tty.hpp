#pragma once

#include <common.hpp>

int kprintf(const char* fmt, ...);
int kdprintf(const char* fmt, ...);

#define KMPRINTF(fmt, ...) \
    do { \
        kprintf(fmt, ##__VA_ARGS__); \
        kdprintf(fmt, ##__VA_ARGS__); \
    } while (0)

#if SPHYNX_MIRROR_PRINTF
#define printf(fmt, ...) KMPRINTF(fmt, ##__VA_ARGS__)
#else
#define printf(fmt, ...) kprintf(fmt, ##__VA_ARGS__)
#endif

#define _DNEWLINE() \
    do { \
        kdprintf("\n"); \
    } while (0)

#define DINFO(fmt, ...) \
    do { \
        kdprintf("info=> "); \
        kdprintf(fmt, ##__VA_ARGS__); \
        _DNEWLINE(); \
    } while (0)

#define DOK(fmt, ...) \
    do { \
        kdprintf("ok=> "); \
        kdprintf(fmt, ##__VA_ARGS__); \
        _DNEWLINE(); \
    } while (0)

#define DWARN(fmt, ...) \
    do { \
        kdprintf("warning=> "); \
        kdprintf(fmt, ##__VA_ARGS__); \
        _DNEWLINE(); \
    } while (0)

#define DERROR(fmt, ...) \
    do { \
        kdprintf("error=> "); \
        kdprintf(fmt, ##__VA_ARGS__); \
        _DNEWLINE(); \
    } while (0)
