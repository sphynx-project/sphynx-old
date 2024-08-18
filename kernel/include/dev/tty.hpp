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

void vprintf(const char* fmt, va_list args);

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
#define DPRINTF(fmt, ...) KMPRINTF(fmt, ##__VA_ARGS__)
#else
#define DPRINTF(fmt, ...) kdprintf(fmt, ##__VA_ARGS__)
#endif

class Logger {
public:
    enum Level {
        DEBUG,
        INFO,
        OK,
        WARN,
        ERROR,
    };

    Logger(const char* name) : name(name), level(INFO) {}

    void set_level(Level lvl) {
        level = lvl;
    }

    void log(Level lvl, const char* fmt, ...) const {
        if (lvl < level) return;

        const char* color = get_color(lvl);
        const char* kind = get_kind(lvl);
        printf("%s[%-6s] [%-10s] ", color, kind, name);

        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        printf("\033[0m");
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
            case INFO: return "\x1b[38;2;210;180;140m";
            case OK: return "\x1b[38;2;205;133;63m";
            case WARN: return "\x1b[38;2;218;165;32m";
            case ERROR: return "\x1b[38;2;139;69;19m";
            case DEBUG: return "\x1b[38;2;160;82;45m";
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
            default: return "UNKNOWN";
        }
    }
};