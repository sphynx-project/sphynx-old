#include <stdint.h>

void hlt()
{
    __asm__ volatile("hlt");
    for (;;)
        ;
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void putc(char ch) {
    outb(0xE9, ch);
}

void print(const char* str) {
    char* data = (char*)str;
    while(*data) {
        putc(*data);
        data++;
    }
}  

void _start() {
    print("\033c");
    print("info: Kernel loaded");
    hlt();
}