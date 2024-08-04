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

    static const char* reasons[32] = {
        "Division by Zero",
        "Debug",
        "Non-Maskable-Interrupt",
        "Breakpoint",
        "Overflow",
        "Bound Range Exceeded",
        "Invalid opcode",
        "Device (FPU) not available",
        "Double Fault",
        "Resereved Vector",
        "Invalid TSS",
        "Segment not present",
        "Stack Segment Fault",
        "General Protection Fault ",
        "Page Fault ",
        "Resereved Vector",
        "x87 FP Exception",
        "Alignment Check",
        "Machine Check (Internal Error)",
        "SIMD FP Exception",
        "Virtualization Exception",
        "Control  Protection Exception",
        "Resereved Vector",
        "Resereved Vector",
        "Resereved Vector",
        "Resereved Vector",
        "Resereved Vector",
        "Resereved Vector",
        "Hypervisor Injection Exception",
        "VMM Communication Exception",
        "Security Exception",
        "Resereved Vector"
    };

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

    extern "C" void excp_handler(IDT::int_frame_t frame) {
        if(frame.vector < 0x20) {
            kpanic(&frame, reasons[frame.vector]);
            hcf();
        } else if(frame.vector >= 0x20 && frame.vector <= 0x2f) {
            // TODO: IRQs
        } else if(frame.vector == 0x80) {
            // TODO: System Calls
        }
    }
}
