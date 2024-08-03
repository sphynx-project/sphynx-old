#include <sphynxboot.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <stdint.h>
#include <stddef.h>

void * memset(void *d, int c, size_t n) {
  char *p = (char *)d;
  while (n--)
    {
      *p++ = c;
    }
  return d;
}

void *memcpy(void *dest, const void *src, size_t n) {
  char *p1 = (char *)dest;
  char *p2 = (char *)src;

  while (n--)
    {
      *p1++ = *p2++;
    }
  return dest;
}

void hlt()
{
    __asm__ volatile("hlt");
    for (;;)
        ;
}

void hcf() {
    __asm__ volatile("cli");
    for(;;)
        hlt();
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void _start(boot_t *data) {
    if(data == NULL) {
        outb(0xE9, 'E');
        hcf();
    }

    if(data->framebuffer->address == 0) {
        outb(0xE9, 'E');
        hcf();
    }

    struct flanterm_context *ft_ctx = flanterm_fb_init(NULL, NULL, data->framebuffer->address, data->framebuffer->width, data->framebuffer->height, data->framebuffer->pitch, data->framebuffer->red_mask_size, data->framebuffer->red_mask_shift, data->framebuffer->green_mask_size, data->framebuffer->green_mask_shift, data->framebuffer->blue_mask_size, data->framebuffer->blue_mask_shift, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 1, 0, 0, 0);
    if(ft_ctx == NULL) {
        outb(0xE9, 'E');
        hlt();
    }

    ft_ctx->raw_putchar(ft_ctx, 'A');

    hlt();
}