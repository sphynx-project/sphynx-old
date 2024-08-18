/*
Sphynx Operating System

File: kernel.cpp
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

Description: Sphynx kernel entry
*/

#include <common.hpp>
#include <stdint.h>
#include <dev/tty.hpp>
#include <sys/cpu.hpp>
#include <core/gdt.hpp>
#include <core/idt.hpp>
#include <core/mm/pmm.hpp>
#include <external/seif.h>

struct flanterm_context* ftCtx;
struct boot *bootInfo;
struct file *ramfs;
struct framebuffer *framebuffer;


void putpixel(framebuffer_t *fb, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x >= fb->width || y >= fb->height) {
        return;
    }

    uint32_t *pixel_addr = (uint32_t *)(fb->address + y * fb->pitch + x * (fb->bpp / 8));

    uint32_t color = 0xFF000000 | (r << 16) | (g << 8) | b;

    *pixel_addr = color;
}

void draw_image(framebuffer_t *fb, uint8_t *image) {
    Logger logger("SeifImage");
    SEIF_Header *header = (SEIF_Header *)image;
    if(header->magic[0] != 'S' || header->magic[1] != 'E' || header->magic[2] != 'I' || header->magic[3] != 'F') {
        logger.log(Logger::Level::ERROR, "Failed to draw image: Invalid image magic!\n");
        return;
    }
    
    uint8_t encoding_size = 0;
    if(header->encoding == SEIF_ENCODING_RGB) {
        encoding_size = 3;
    } else if(header->encoding == SEIF_ENCODING_RGBA || header->encoding == SEIF_ENCODING_ARGB) {
        encoding_size = 4;
    }

    SEIF_ChunkHeader *chunk_header = (SEIF_ChunkHeader *)(image + sizeof(SEIF_Header));
    if(header->chunk_count != 1 || 
       chunk_header->width != header->meta.width || 
       chunk_header->height != header->meta.height) {
        logger.log(Logger::Level::ERROR, "Unsupported image format or multiple chunks!\n");
        return;
    }

    uint8_t *data = (uint8_t *)(image + sizeof(SEIF_Header) + sizeof(SEIF_ChunkHeader));

    for(int y = 0; y < header->meta.height; y++) {
        for(int x = 0; x < header->meta.width; x++) {
            int index = (y * header->meta.width + x) * encoding_size;

            uint8_t r = 0, g = 0, b = 0, a = 255;

            if(header->encoding == SEIF_ENCODING_RGB) {
                r = data[index];
                g = data[index + 1];
                b = data[index + 2];
            } else if(header->encoding == SEIF_ENCODING_RGBA) {
                r = data[index];
                g = data[index + 1];
                b = data[index + 2];
                a = data[index + 3];
            } else if(header->encoding == SEIF_ENCODING_ARGB) {
                a = data[index];
                r = data[index + 1];
                g = data[index + 2];
                b = data[index + 3];
            }

            (void)a;
            putpixel(fb, x, y, r, g, b);
        }
    }
}

extern "C" void _start(boot_t* data) {
    kdprintf("\033c");
    if (!data) {
        kdprintf(" - Error: Failed to get bootdata\n");
        hcf();
    }

    if(!data->framebuffer || data->framebuffer->address == 0) {
        kdprintf(" - Error: Failed to get framebuffer\n");
        hcf();
    }

    bootInfo = data;
    framebuffer = data->framebuffer;

    uint32_t defaultBg = 0x0b0500;
	uint32_t defaultFg = 0xffffff;

    ftCtx = flanterm_fb_init(
        nullptr, nullptr, reinterpret_cast<uint32_t*>(framebuffer->address),
        framebuffer->width, framebuffer->height,
        framebuffer->pitch, framebuffer->red_mask_size,
        framebuffer->red_mask_shift, framebuffer->green_mask_size,
        framebuffer->green_mask_shift, framebuffer->blue_mask_size,
        framebuffer->blue_mask_shift, nullptr, nullptr, nullptr, &defaultBg,
        &defaultFg, nullptr, nullptr, nullptr, 0, 0, 1, 1, 1, 5
    );

    if (!ftCtx) {
        kdprintf("- Error: Failed to initialize flanterm\n");
        hcf();
    }

    ftCtx->cursor_enabled = false;
    ftCtx->full_refresh(ftCtx);
    Logger logger("SphynxMain");
    #if SPHYNX_DEBUG
    logger.set_level(Logger::Level::DEBUG);
    #else
    logger.set_level(Logger::Level::INFO);
    #endif

    uint8_t *image = (uint8_t*)data->ramfs->address;

    draw_image(data->framebuffer, image);

    logger.log(Logger::Level::INFO, "Flanterm Initialized\n");
    

    GDT::init();
    logger.log(Logger::Level::INFO, "GDT Initialized\n");
    IDT::init();
    logger.log(Logger::Level::INFO, "IDT Initialized\n");
    

    if(data->ramfs == nullptr) {
        kpanic(nullptr, "Sphynx got no ramfs, expected ramfs in /sphynx/ramfs");
    }

    ramfs = data->ramfs;
    logger.log(Logger::Level::INFO, "ramfs loaded\n");

    if(data->memory_map == nullptr) {
        kpanic(nullptr, "Failed to get memory map");
    }
    logger.log(Logger::Level::INFO, "Memory map loaded\n");
    logger.log(Logger::Level::DEBUG, "Screen Size: ");
    printf("%dx%d\n", framebuffer->width, framebuffer->height);
    logger.log(Logger::Level::DEBUG, "Bootloader: ");
    printf("%s\n", bootInfo->info->name);

    halt();
}
