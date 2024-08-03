#include <sphynxboot.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <stdint.h>
#include <stddef.h>

struct flanterm_context* ft_ctx;

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

void dprint(const char* str) {
    while(*str)
        outb(0xE9, *str++);
}

void _putc(char ch) {
    flanterm_write(ft_ctx, &ch, sizeof(ch));
}

void print(const char* str) {
    while(*str)
        _putc(*str++);
}

extern "C" void _start(boot_t* data) {
    if (!data || data->framebuffer->address == 0) {
        dprint("ERROR: Failed to get ");
        if(!data)
            dprint("boot info");
        else
            dprint("framebuffer");
        dprint("\n");
        hcf();
    }

    ft_ctx = flanterm_fb_init(
        nullptr, nullptr, reinterpret_cast<uint32_t *>(data->framebuffer->address),
        data->framebuffer->width, data->framebuffer->height,
        data->framebuffer->pitch, data->framebuffer->red_mask_size,
        data->framebuffer->red_mask_shift, data->framebuffer->green_mask_size,
        data->framebuffer->green_mask_shift, data->framebuffer->blue_mask_size,
        data->framebuffer->blue_mask_shift, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, 0, 0, 1, 0, 0, 0
    );
    
    if (!ft_ctx) {
        dprint("ERROR: Failed to initialize flanterm\n");
        hlt();
    }

    ft_ctx->cursor_enabled = false;
    ft_ctx->full_refresh(ft_ctx);

    print("Sphynx v0.0.1 (Bootloader: ");
    print(data->info->name);
    print(")\n");

    hlt();
}