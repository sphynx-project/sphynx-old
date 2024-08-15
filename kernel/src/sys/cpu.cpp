/*
Sphynx Operating System

File: cpu.cpp
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

Description: Common CPU functions and utilities
*/

#include <sys/cpu.hpp>
#include <dev/tty.hpp>
#include <string.hpp>

void _kpanic_print_reg(const char* name, uint64_t value) {
    DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", name, value);
}

void _kpanic_handler(IDT::int_frame_t *frame, const char* file, int line, const char* reason) {
    KMPRINTF("\033[31mKernel panic at \"%s:%d\", Reason: \"%s\"!\033[0m\n", file, line, reason);
    DPRINTF("\033[31m--------------------\033[0m\n");

    if (frame != nullptr) {

        DPRINTF("\033[31mREGISTER STATE:\033[0m\n");
        _kpanic_print_reg("CR2", frame->cr2);
        _kpanic_print_reg("CR3", frame->cr3);
        _kpanic_print_reg("RAX", frame->rax);
        _kpanic_print_reg("RBX", frame->rbx);
        _kpanic_print_reg("RCX", frame->rcx);
        _kpanic_print_reg("RDX", frame->rdx);
        _kpanic_print_reg("RSI", frame->rsi);
        _kpanic_print_reg("RDI", frame->rdi);
        _kpanic_print_reg("RBP", frame->rbp);
        _kpanic_print_reg("R8 ", frame->r8);
        _kpanic_print_reg("R9 ", frame->r9);
        _kpanic_print_reg("R10", frame->r10);
        _kpanic_print_reg("R11", frame->r11);
        _kpanic_print_reg("R12", frame->r12);
        _kpanic_print_reg("R13", frame->r13);
        _kpanic_print_reg("R14", frame->r14);
        _kpanic_print_reg("R15", frame->r15);
        _kpanic_print_reg("RIP", frame->rip);
        _kpanic_print_reg("CS ", frame->cs);
        _kpanic_print_reg("RFLAGS", frame->rflags);
        _kpanic_print_reg("RSP", frame->rsp);
        _kpanic_print_reg("SS ", frame->ss);
        _kpanic_print_reg("DS ", frame->ds);
        _kpanic_print_reg("ERR", frame->err);
        _kpanic_print_reg("VECTOR", frame->vector);

        if (frame->vector == 14) {
            DPRINTF("\033[31mEXCEPTION INFO:\033[0m\n");
            DPRINTF("\033[31m  Page Fault Details:\033[0m\n");
            DPRINTF("    Operation:        %s\n", (frame->err & 0x1) ? "Protection Violation" : "Non-Present Page");
            DPRINTF("    Access Type:      %s\n", (frame->err & 0x2) ? "Write" : "Read");
            DPRINTF("    Privilege Level: %s\n", (frame->err & 0x4) ? "User Mode" : "Supervisor Mode");
            if (frame->err & 0x8) DPRINTF("    Reserved Write:  Yes\n");
            if (frame->err & 0x10) DPRINTF("    Instruction Fetch: Yes\n");
        } else {
            DPRINTF("\033[31mEXCEPTION INFO:\033[0m\n");
            DPRINTF("  Interrupt Number:  %lu\n", frame->vector);
            DPRINTF("  Error Code:        0x%016llx\n", frame->err);
        }
    }

    kdprintf("\033[31mSTACK TRACE:\n\033[0m");
    kdprintf("\033[31m  (TODO)\n");

    DPRINTF("\033[31m\033[0m");

    hcf();
}
