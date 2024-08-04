/*
Sphynx Operating System

File: gdt.cpp
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

Description: Sphynx GDT
*/

#include <core/gdt.hpp>

namespace GDT {
	gdtr_t gdtr;

	struct __packed {
		descriptor_t entries[5];
	} gdt;

	void init() {
		gdt.entries[0] = (descriptor_t){0,0,0,0,0,0};
		gdt.entries[1] = (descriptor_t){0, 0, 0, 0b10011010, 0b10100000, 0};
		gdt.entries[2] = (descriptor_t){0, 0, 0, 0b10010010, 0b10100000, 0};
		gdt.entries[3] = (descriptor_t){0, 0, 0, 0b11111010, 0b10100000, 0};
		gdt.entries[4] = (descriptor_t){0, 0, 0, 0b11110010, 0b10100000, 0};
		gdtr.size = (uint16_t)(sizeof(gdt) - 1);
		gdtr.offset = (uintptr_t)&gdt;
		reload();
	}

	void reload() {
	    asm volatile (
	        "mov %0, %%rdi\n"
	        "lgdt (%%rdi)\n"
	        "push $0x8\n"
	        "lea 1f(%%rip), %%rax\n"
	        "push %%rax\n"
	        "lretq\n"
	        "1:\n"
	        "mov $0x10, %%ax\n"
	        "mov %%ax, %%es\n"
	        "mov %%ax, %%ss\n"
	        "mov %%ax, %%ds\n"
	        :
	        : "r" (&gdtr)
	        : "memory"
	    );
	}
}