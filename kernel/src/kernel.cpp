#include <common.hpp>
#include <stdint.h>
#include <dev/tty.hpp>
#include <sys/cpu.hpp>

struct flanterm_context* ftCtx;
struct boot *boot_info;
struct framebuffer *framebuffer;

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void debug_print(const char* str) {
    while (*str) {
        outb(0xE9, *str++);
    }
}

extern "C" void _start(boot_t* data) {
    if (!data || !data->framebuffer || data->framebuffer->address == 0) {
        debug_print("ERROR: Failed to get ");
        debug_print(!data ? "boot info" : "framebuffer");
        debug_print("\n");
        hcf();
    }

    boot_info = data;
    framebuffer = data->framebuffer;

    ftCtx = flanterm_fb_init(
        nullptr, nullptr, reinterpret_cast<uint32_t*>(framebuffer->address),
        framebuffer->width, framebuffer->height,
        framebuffer->pitch, framebuffer->red_mask_size,
        framebuffer->red_mask_shift, framebuffer->green_mask_size,
        framebuffer->green_mask_shift, framebuffer->blue_mask_size,
        framebuffer->blue_mask_shift, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, 0, 0, 1, 0, 0, 0
    );

    if (!ftCtx) {
        debug_print("ERROR: Failed to initialize flanterm\n");
        halt();
    }

    ftCtx->cursor_enabled = false;
    ftCtx->full_refresh(ftCtx);

    printf("Sphynx v0.0.1 (Bootloader: %s)\n", data->info->name);
    printf(" - Screen: %dx%d\n", framebuffer->width, framebuffer->height);
    halt();
}
