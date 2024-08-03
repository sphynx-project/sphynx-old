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

void putc(char ch) {
    outb(0xE9, ch);
}

void puts(const char* str) {
    if (str == nullptr) {
        putc('R');
        return;
    }

    if (*str == '\0') {
        putc('E');
        return;
    }

    while (*str) {
        putc(*str++);
    }
}

extern "C" void _start(boot_t* data) {
    if (!data || data->framebuffer->address == 0) {
        putc('E');
        hcf();
    }

    
    puts("Hello, World!");

    hlt();
}
