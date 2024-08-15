/*
Sphynx Operating System

File: tty.hpp
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

#pragma once

#include <common.hpp>
#include <stdarg.h>

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

#if SPHYNX_VERBOSE
#define DPRINTF KMPRINTF
#else
#define DPRINTF kdprintf
#endif

class Logger {
public:
    enum Level {
        INFO,
        OK,
        WARN,
        ERROR,
        DEBUG
    };

    Logger(const char* name) : name(name), level(INFO) {}

    void set_level(Level lvl) {
        level = lvl;
    }

    void log(Level lvl, const char* fmt, ...) const {
        if (lvl < level) return;

        const char* color = get_color(lvl);
        const char* kind = get_kind(lvl);
        if(lvl == DEBUG) {
            kdprintf("%s[%s] [%s] ", color, kind, name);

            va_list args;
            va_start(args, fmt);
            kdprintf(fmt, args);
            va_end(args);

            kdprintf("\033[0m");
        } else {
            printf("%s[%s] [%s] ", color, kind, name);

            va_list args;
            va_start(args, fmt);
            printf(fmt, args);
            va_end(args);

            printf("\033[0m");
        }

    }

    void info(const char* fmt, ...) const {
        va_list args;
        va_start(args, fmt);
        log(INFO, fmt, args);
        va_end(args);
    }

    void ok(const char* fmt, ...) const {
        va_list args;
        va_start(args, fmt);
        log(OK, fmt, args);
        va_end(args);
    }

    void warn(const char* fmt, ...) const {
        va_list args;
        va_start(args, fmt);
        log(WARN, fmt, args);
        va_end(args);
    }

    void error(const char* fmt, ...) const {
        va_list args;
        va_start(args, fmt);
        log(ERROR, fmt, args);
        va_end(args);
    }

    void debug(const char* fmt, ...) const {
        va_list args;
        va_start(args, fmt);
        log(DEBUG, fmt, args);
        va_end(args);
    }

private:
    const char* name;
    Level level;

    const char* get_color(Level lvl) const {
        switch (lvl) {
            case INFO: return "\033[0m";
            case OK: return "\033[32m";
            case WARN: return "\033[33m";
            case ERROR: return "\033[31m";
            case DEBUG: return "\033[35m";
            default: return "\033[0m";
        }
    }

    const char* get_kind(Level lvl) const {
        switch (lvl) {
            case INFO: return "INFO";
            case OK: return "OK";
            case WARN: return "WARN";
            case ERROR: return "ERROR";
            case DEBUG: return "DEBUG";
            default: return "UNKOWN";
        }
    }
};