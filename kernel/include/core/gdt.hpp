/*
Sphynx Operating System

File: gdt.hpp
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

#pragma once

#include <stdint.h>
#include <common.hpp>

namespace GDT {
	typedef struct {
	    uint16_t limit_low;
	    uint16_t base_low;
	    uint8_t base_mid;
	    uint8_t access_byte;
	    uint8_t limit_high_and_flags;
	    uint8_t base_high;
	} __packed descriptor_t;

	typedef struct {
	    uint16_t limit_low;
	    uint16_t base_low;
	    uint8_t base_mid;
	    uint8_t access_byte;
	    uint8_t limit_high_and_flags;
	    uint8_t base_high;
	    uint32_t base;
	    uint32_t reserved;
	} __packed descriptor_ex_t;

	typedef struct {
	    uint16_t size;
	    uintptr_t offset;
	} __packed gdtr_t;

	extern gdtr_t gdtr;

	void init();
	void reload();
}