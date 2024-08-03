#include <sphynxboot.h>
#include <stdint.h>
#include <stddef.h>

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

void puts(char* str) {
    while (*str)
        outb(0xE9, *str++);
}

extern "C" void _start(boot_t* data) {
    if (!data || data->framebuffer->address == 0) {
        outb(0xE9, 'E');
        hcf();
    }

    puts("Hello, World!\n");


    hlt();
}
