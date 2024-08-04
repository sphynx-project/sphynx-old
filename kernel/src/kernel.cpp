#include <common.hpp>
#include <stdint.h>
#include <dev/tty.hpp>

struct flanterm_context* ftCtx;

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

    tty_write("Sphynx v0.0.1 (Bootloader: ");
    tty_write(data->info->name);
    tty_write(")\n");
    tty_write(" - Screen: ");
    tty_write(data->framebuffer->width);
    tty_write("x");
    tty_write(data->framebuffer->height);
    tty_write("\n");

    halt();
}
