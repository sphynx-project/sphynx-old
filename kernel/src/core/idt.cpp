/*
Sphynx Operating System

File: idt.cpp
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

Description: Sphynx IDT
*/

#include <core/idt.hpp>
#include <sys/cpu.hpp>
#include <dev/tty.hpp>

namespace IDT {
    #define IDT_ENTRIES 256

    extern "C" uint64_t isrTable[];
    extern "C" void load_idt(uint64_t);

    IDT::idt_entry_t idt[IDT_ENTRIES];
    IDT::idt_pointer_t idt_p;

    void set_gate(uint8_t interrupt, uintptr_t base, int8_t flags) {
        idt_entry_t *descriptor = &idt[interrupt];

        descriptor->offsetLow = base & 0xFFFF;
        descriptor->selector = 0x8;
        descriptor->ist = 0;
        descriptor->flags = flags;
        descriptor->offsetMiddle = (base >> 16) & 0xFFFF;
        descriptor->offsetHigh = (base >> 32) & 0xFFFFFFFF;
        descriptor->zero = 0;
    }

    void init() {
        idt_p.limit = sizeof(IDT::idt_entry_t) * IDT_ENTRIES - 1;
        idt_p.base = (uint64_t)&idt;

        __asm__ volatile("sti");
        for(int i = 0; i < 32; i++) {
            set_gate(i, isrTable[i], 0b10001111);
        }

        for(int i = 32; i < IDT_ENTRIES; i++) {
            set_gate(i, isrTable[i], 0b10001110);
        }

        load_idt((uint64_t)&idt_p);
        __asm__ volatile("cli");
    }

    void capture_regs(int_frame_t *context) {
        asm volatile (
            "movq %%rax, %0\n\t"
            "movq %%rbx, %1\n\t"
            "movq %%rcx, %2\n\t"
            "movq %%rdx, %3\n\t"
            "movq %%rsi, %4\n\t"
            "movq %%rdi, %5\n\t"
            "movq %%rbp, %6\n\t"
            "movq %%r8,  %7\n\t"
            "movq %%r9,  %8\n\t"
            "movq %%r10, %9\n\t"
            "movq %%r11, %10\n\t"
            "movq %%r12, %11\n\t"
            "movq %%r13, %12\n\t"
            "movq %%r14, %13\n\t"
            "movq %%r15, %14\n\t"
            : "=m" (context->rax), "=m" (context->rbx), "=m" (context->rcx), "=m" (context->rdx),
            "=m" (context->rsi), "=m" (context->rdi), "=m" (context->rbp), "=m" (context->r8),
            "=m" (context->r9), "=m" (context->r10), "=m" (context->r11), "=m" (context->r12),
            "=m" (context->r13), "=m" (context->r14), "=m" (context->r15)
            :
            : "memory"
        );

        asm volatile (
            "movq %%cs,  %0\n\t"
            "movq %%ss,  %1\n\t"
            "movq %%es,  %2\n\t"
            "movq %%ds,  %3\n\t"
            "movq %%cr0, %4\n\t"
            "movq %%cr2, %5\n\t"
            "movq %%cr3, %6\n\t"
            "movq %%cr4, %7\n\t"
            : "=r" (context->cs), "=r" (context->ss), "=r" (context->es), "=r" (context->ds),
            "=r" (context->cr0), "=r" (context->cr2), "=r" (context->cr3), "=r" (context->cr4)
            :
            : "memory"
        );

        asm volatile (
            "movq %%rsp, %0\n\t"
            "pushfq\n\t"
            "popq %1\n\t"
            : "=r" (context->rsp), "=r" (context->rflags)
            :
            : "memory"
        );

        context->rip = (uint64_t)__builtin_return_address(0);
    }

    extern "C" void excp_handler(IDT::int_frame_t frame) {
        if(frame.vector < 0x20) {
            printf("panic @ 0x%.16llx (vector: 0x%.2x)\n", frame.rip, frame.vector);
            hcf();
        } else if(frame.vector >= 0x20 && frame.vector <= 0x2f) {
            // TODO: IRQs
        } else if(frame.vector == 0x80) {
            // TODO: System Calls
        }
    }
}
