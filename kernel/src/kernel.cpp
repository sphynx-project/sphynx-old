#include <sphynxboot.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

struct flanterm_context* ftCtx;

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

[[noreturn]] void halt() {
    __asm__ volatile("hlt");
    while (true) { }
}

[[noreturn]] void hcf() {
    __asm__ volatile("cli");
    halt();
}

void out_byte(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void debug_print(const char* str) {
    while (*str) {
        out_byte(0xE9, *str++);
    }
}

void putc(char ch) {
    flanterm_write(ftCtx, &ch, sizeof(ch));
}

void print(const char* str) {
    while (*str) {
        putc(*str++);
    }
}

void itoa(int value, char* buffer, int base) {
    char* ptr = buffer;
    char* ptr1 = buffer;
    char tmpChar;
    int tmpValue;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    bool isNegative = (value < 0 && base == 10);
    if (isNegative) {
        value = -value;
    }

    tmpValue = value;
    while (tmpValue) {
        int rem = tmpValue % base;
        *ptr++ = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        tmpValue /= base;
    }

    if (isNegative) {
        *ptr++ = '-';
    }
    *ptr = '\0';

    while (ptr1 < --ptr) {
        tmpChar = *ptr1;
        *ptr1++ = *ptr;
        *ptr = tmpChar;
    }
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[32];

    for (const char* p = fmt; *p; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 's':
                    print(va_arg(args, const char*));
                    break;
                case 'd':
                    itoa(va_arg(args, int), buffer, 10);
                    print(buffer);
                    break;
                default:
                    putc('%');
                    putc(*p);
                    break;
            }
        } else {
            putc(*p);
        }
    }

    va_end(args);
}

extern "C" void _start(boot_t* data) {
    if (!data || !data->framebuffer || data->framebuffer->address == 0) {
        debug_print("ERROR: Failed to get ");
        debug_print(!data ? "boot info" : "framebuffer");
        debug_print("\n");
        hcf();
    }

    ftCtx = flanterm_fb_init(
        nullptr, nullptr, reinterpret_cast<uint32_t*>(data->framebuffer->address),
        data->framebuffer->width, data->framebuffer->height,
        data->framebuffer->pitch, data->framebuffer->red_mask_size,
        data->framebuffer->red_mask_shift, data->framebuffer->green_mask_size,
        data->framebuffer->green_mask_shift, data->framebuffer->blue_mask_size,
        data->framebuffer->blue_mask_shift, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, 0, 0, 1, 0, 0, 0
    );

    if (!ftCtx) {
        debug_print("ERROR: Failed to initialize flanterm\n");
        halt();
    }

    ftCtx->cursor_enabled = false;
    ftCtx->full_refresh(ftCtx);

    printf("Sphynx v0.0.1 (Bootloader: %s)\n", data->info->name);
    printf("- Screen: %dx%d", data->framebuffer->width, data->framebuffer->height);

    halt();
}
