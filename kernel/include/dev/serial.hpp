/*
Sphynx Operating System

File: serial.hpp
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

#pragma once

#include <stdint.h>
#include <common.hpp>

namespace Serial {
	// Common ports
	typedef enum {
		COM1 = 0x3F8,
		COM2 = 0x2F8,
		COM3 = 0x3E8,
		COM4 = 0x2E8,
		COM5 = 0x5F8,
		COM6 = 0x4F8,
		COM7 = 0x5E8,
		COM8 = 0x4E8
	} COM_PORTS;

	typedef enum {
		_COM1 = COM_PORTS::COM1,
		_COM2 = COM_PORTS::COM2,
		_COM3 = COM_PORTS::COM3,
		_COM4 = COM_PORTS::COM4,
		_COM5 = COM_PORTS::COM5,
		_COM6 = COM_PORTS::COM6,
		_COM7 = COM_PORTS::COM7,
		_COM8 = COM_PORTS::COM8
	} KNOWN_PORTS;

	static constexpr uint8_t ERROR_FAILED_TO_INIT = 0x01; 

	class Stream {
	public:
		Stream(COM_PORTS port);
		uint8_t read();
		void write(uint8_t data);
		bool has_error();
		char* error_to_str();
		
	private:
		bool serial_recived();
		bool is_transmit_empty();

	private:
		COM_PORTS port;
		bool is_error;
		uint8_t error;
	};

	void outb(uint16_t port, uint8_t value);
	void outw(uint16_t port, uint16_t value);
	void outd(uint16_t port, uint32_t value);
	uint8_t inb(uint16_t port);
	uint16_t inw(uint16_t port);
	uint32_t ind(uint16_t port);
}