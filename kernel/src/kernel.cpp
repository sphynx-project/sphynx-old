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

struct flanterm_context* ftCtx;
struct boot *bootInfo;
struct file *ramfs;
struct framebuffer *framebuffer;

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

    uint32_t defaultBg = 0x1b1c1b;
	uint32_t defaultFg = 0xffffff;

    ftCtx = flanterm_fb_init(
        nullptr, nullptr, reinterpret_cast<uint32_t*>(framebuffer->address),
        framebuffer->width, framebuffer->height,
        framebuffer->pitch, framebuffer->red_mask_size,
        framebuffer->red_mask_shift, framebuffer->green_mask_size,
        framebuffer->green_mask_shift, framebuffer->blue_mask_size,
        framebuffer->blue_mask_shift, nullptr, nullptr, nullptr, &defaultBg,
        &defaultFg, nullptr, nullptr, nullptr, 0, 0, 1, 1, 1, 0
    );

    if (!ftCtx) {
        kdprintf("- Error: Failed to initialize flanterm\n");
        hcf();
    }

    ftCtx->cursor_enabled = false;
    ftCtx->full_refresh(ftCtx);
    DINFO("Flanterm Initialized");

    GDT::init();
    DINFO("GDT Initialized");
    IDT::init();
    DINFO("IDT Initialized");
    

    if(data->ramfs == nullptr) {
        kpanic(nullptr, "Sphynx got no ramfs, expected ramfs in /sphynx/ramfs");
    }

    ramfs = data->ramfs;
    DINFO("ramfs loaded");
    printf("%.*s (%s)\n", ramfs->size, static_cast<char*>(ramfs->address), data->info->name);

    if(data->memory_map == nullptr) {
        kpanic(nullptr, "Failed to get memory map");
    }

    
    memory_map_t *memory_map = data->memory_map;
    PMM::init(memory_map);
    printf("%d bytes FREE\n", PMM::get_free());

    void* ptr = PMM::request_pages(2);
    if(ptr == nullptr)
        kpanic(nullptr, "Failed to test allocate");

    halt();
}
