/*
Sphynx Operating System

File: serial.cpp
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

Description: Sphynx serial I/O manager
*/

#include <dev/serial.hpp>

namespace Serial {
	Stream::Stream(COM_PORTS port) : port(port) {
		outb(port + 1, 0x00);
	    outb(port + 3, 0x80);
	    outb(port + 0, 0x03);
	    outb(port + 1, 0x00);
	    outb(port + 3, 0x03);
	    outb(port + 2, 0xC7);
	    outb(port + 4, 0x0B);
	    outb(port + 4, 0x1E);
	    outb(port + 0, 0x69);
	    if(inb(port + 0) != 0x69) {
	    	error = ERROR_FAILED_TO_INIT;
	    	is_error = true;
	    	return;
	    }

	    outb(port + 4, 0x0F);
	    is_error = false;
	}

	bool Stream::is_transmit_empty() { 
		return inb(port + 5) & 0x20;
	}

	bool Stream::serial_recived() {
		return inb(port + 5) & 0x01;
	}

	uint8_t Stream::read() {
		while(serial_recived() == 0);
		return inb(port);
	}

	void Stream::write(uint8_t data) {
		while(is_transmit_empty() == 0);
		outb(port, data);
	}

	bool Stream::has_error() {
		return is_error;
	}

	char* Stream::error_to_str() {
		switch(error) {
		case ERROR_FAILED_TO_INIT:
			return (char*)"Failed to init port";
		default:
			return (char*)"Unknown error";
		}
	}

	void outb(uint16_t port, uint8_t value) {
	    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
	}

	void outw(uint16_t port, uint16_t value) {
	    __asm__ volatile("outw %1, %0" : : "dN"(port), "a"(value));
	}

	void outd(uint16_t port, uint32_t value) {
	    __asm__ volatile("outl %1, %0" : : "dN"(port), "a"(value));
	}

	uint8_t inb(uint16_t port) {
	    uint8_t r;
	    __asm__ volatile("inb %1, %0" : "=a"(r) : "dN"(port));
	    return r;
	}

	uint16_t inw(uint16_t port) {
	    uint16_t r;
	    __asm__ volatile("inw %1, %0" : "=a"(r) : "dN"(port));
	    return r;
	}

	uint32_t ind(uint16_t port) {
	    uint32_t r;
	    __asm__ volatile("inl %1, %0" : "=a"(r) : "dN"(port));
	    return r;
	}
}