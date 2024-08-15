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

void _kpanic_handler(IDT::int_frame_t *frame, const char* file, int line, const char* reason) {
    #if SPHYNX_SIMPLE_PANIC
    KMPRINTF("\033[31mKernel Panic @ CPU %s (0x%.16llx), Reason: \"%s\", %s:%d\n", "???", (frame == nullptr) ? 0x0 : frame->rip, reason, file, line);
    #else
    KMPRINTF("\n\033[31mKernel panic - cpu %s: %s\033[0m\n", "???", reason);
    KMPRINTF("\033[31mIn file: %s, line: %d\033[0m\n", file, line);

    #if SPHYNX_VERBOSE_IDT
    if (frame != nullptr) {
        DPRINTF("\033[31mRegister dump:\033[0m\n");
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "CR2", frame->cr2);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "CR3", frame->cr3);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RAX", frame->rax);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RBX", frame->rbx);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RCX", frame->rcx);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RDX", frame->rdx);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RSI", frame->rsi);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RDI", frame->rdi);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RBP", frame->rbp);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "R8 ", frame->r8);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "R9 ", frame->r9);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "R10", frame->r10);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "R11", frame->r11);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "R12", frame->r12);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "R13", frame->r13);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "R14", frame->r14);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "R15", frame->r15);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RIP", frame->rip);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "CS ", frame->cs);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RFLAGS", frame->rflags);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RSP", frame->rsp);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "SS ", frame->ss);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "DS ", frame->ds);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "ERR", frame->err);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "VECTOR", frame->vector);
    }
    #else
    #if SPHYNX_DUMP_REG_ON_INT
    if (frame != nullptr) {
        DPRINTF("\033[31mBasic register dump:\033[0m\n");
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RIP", frame->rip);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RSP", frame->rsp);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "RFLAGS", frame->rflags);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "CS ", frame->cs);
        DPRINTF("\033[31m  %-12s: 0x%016llx\033[0m\n", "SS ", frame->ss);
    }
    #endif
    #endif

    if (frame != nullptr && frame->vector == 14) {
        DPRINTF("\033[31mPage Fault Details:\033[0m\n");
        DPRINTF("\033[31m  Operation: %s\033[0m\n", (frame->err & 0x1) ? "Protection Violation" : "Non-Present Page");
        DPRINTF("\033[31m  Access Type: %s\033[0m\n", (frame->err & 0x2) ? "Write" : "Read");
        DPRINTF("\033[31m  Privilege Level: %s\033[0m\n", (frame->err & 0x4) ? "User Mode" : "Supervisor Mode");
        if (frame->err & 0x8) DPRINTF("\033[31m  Reserved Write: Yes\033[0m\n");
        if (frame->err & 0x10) DPRINTF("\033[31m  Instruction Fetch: Yes\033[0m\n");
    }

    KMPRINTF("\033[31mStack trace:\033[0m\n");
    KMPRINTF("\033[31m  (TODO: Implement stack trace)\033[0m\n");

    #endif

    hcf();
}
