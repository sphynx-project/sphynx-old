#include <sphynxboot.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <stdint.h>
#include <stddef.h>

extern "C" void* memset(void* d, int c, size_t n) {
    auto* p = static_cast<char*>(d);
    while (n--) {
        *p++ = c;
    }
    return d;
}

extern "C" void* memcpy(void* dest, const void* src, size_t n) {
    auto* p1 = static_cast<char*>(dest);
    auto* p2 = static_cast<const char*>(src);
    while (n--) {
        *p1++ = *p2++;
    }
    return dest;
}

[[noreturn]] void hlt() {
    __asm__ volatile("hlt");
    while (true) { }
}

[[noreturn]] void hcf() {
    __asm__ volatile("cli");
    while (true) {
        hlt();
    }
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

extern "C" void _start(boot_t* data) {
    if (!data || data->framebuffer->address == 0) {
        outb(0xE9, 'E');
        hcf();
    }

    auto* ft_ctx = flanterm_fb_init(
        nullptr, nullptr, reinterpret_cast<uint32_t *>(data->framebuffer->address),
        data->framebuffer->width, data->framebuffer->height,
        data->framebuffer->pitch, data->framebuffer->red_mask_size,
        data->framebuffer->red_mask_shift, data->framebuffer->green_mask_size,
        data->framebuffer->green_mask_shift, data->framebuffer->blue_mask_size,
        data->framebuffer->blue_mask_shift, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, 0, 0, 1, 0, 0, 0
    );
    
    if (!ft_ctx) {
        outb(0xE9, 'E');
        hlt();
    }

    ft_ctx->raw_putchar(ft_ctx, 'A');
    hlt();
}
