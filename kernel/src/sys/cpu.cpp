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
    KMPRINTF("  %-10s: 0x%016llx\n", name, value);
}

void kpanic(IDT::int_frame_t *frame, const char* reason) {
    IDT::int_frame_t localFrame;
    if (frame == nullptr) {
        localFrame = {0xEEEEEEEE};
    } else {
        memcpy(&localFrame, frame, sizeof(IDT::int_frame_t));
    }

    ftCtx->set_text_bg_rgb(ftCtx, 0xFF000000); // Black background
    ftCtx->set_text_fg_rgb(ftCtx, 0xFFFF0000); // Red foreground
    ftCtx->clear(ftCtx, true);

    KMPRINTF("================================== KERNEL PANIC ==================================\n");
    KMPRINTF("Location:    0x%.16llx\n", localFrame.rip);
    KMPRINTF("Reason:      %s (0x%.2x)\n", reason, localFrame.vector);
    KMPRINTF("----------------------------------------------------------------------------------\n");

    if(frame != nullptr) {
        KMPRINTF("REGISTER STATE:\n");
        _kpanic_print_reg("CR2", localFrame.cr2);
        _kpanic_print_reg("CR3", localFrame.cr3);
        _kpanic_print_reg("RAX", localFrame.rax);
        _kpanic_print_reg("RBX", localFrame.rbx);
        _kpanic_print_reg("RCX", localFrame.rcx);
        _kpanic_print_reg("RDX", localFrame.rdx);
        _kpanic_print_reg("RSI", localFrame.rsi);
        _kpanic_print_reg("RDI", localFrame.rdi);
        _kpanic_print_reg("RBP", localFrame.rbp);
        _kpanic_print_reg("R8 ", localFrame.r8);
        _kpanic_print_reg("R9 ", localFrame.r9);
        _kpanic_print_reg("R10", localFrame.r10);
        _kpanic_print_reg("R11", localFrame.r11);
        _kpanic_print_reg("R12", localFrame.r12);
        _kpanic_print_reg("R13", localFrame.r13);
        _kpanic_print_reg("R14", localFrame.r14);
        _kpanic_print_reg("R15", localFrame.r15);
        _kpanic_print_reg("RIP", localFrame.rip);
        _kpanic_print_reg("CS ", localFrame.cs);
        _kpanic_print_reg("RFLAGS", localFrame.rflags);
        _kpanic_print_reg("RSP", localFrame.rsp);
        _kpanic_print_reg("SS ", localFrame.ss);
        _kpanic_print_reg("DS ", localFrame.ds);
        _kpanic_print_reg("ERR", localFrame.err);
        _kpanic_print_reg("VECTOR", localFrame.vector);
        KMPRINTF("----------------------------------------------------------------------------------\n");
        KMPRINTF("EXCEPTION INFO:\n");
        if (localFrame.vector == 14) {
            KMPRINTF("  Page Fault Details:\n");
            KMPRINTF("    Operation:        %s\n", (localFrame.err & 0x1) ? "Protection Violation" : "Non-Present Page");
            KMPRINTF("    Access Type:      %s\n", (localFrame.err & 0x2) ? "Write" : "Read");
            KMPRINTF("    Privilege Level: %s\n", (localFrame.err & 0x4) ? "User Mode" : "Supervisor Mode");
            if (localFrame.err & 0x8) {
                KMPRINTF("    Reserved Write:  Yes\n");
            }
            if (localFrame.err & 0x10) {
                KMPRINTF("    Instruction Fetch: Yes\n");
            }
        } else {
            KMPRINTF("  Interrupt Number:  %lu\n", localFrame.vector);
            KMPRINTF("  Error Code:        0x%016llx\n", localFrame.err);
        }
        KMPRINTF("----------------------------------------------------------------------------------\n");
    }

    KMPRINTF("STACK TRACE:\n");
    KMPRINTF("  (TODO)\n");

    KMPRINTF("==================================================================================\n");
    hcf();
}
