/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */
 
#include <cstdio>
#include <cstdlib>
#include "src/includes/memory.h"
 
// init vars
u8 Memory::mem[0x10000] = {0x00};

// responsible for initializing the memory
void Memory:: Init()
{
	memset(mem, 0x00, sizeof(mem));

	Memory::mem[Address::TIMA] = 0x00;
	Memory::mem[Address::TMA] = 0x00;
	Memory::mem[Address::TAC] = 0x00;
	Memory::mem[Address::LCDC] = 0x91;
	Memory::mem[Address::STAT] = 0x00;
	Memory::mem[Address::SCY] = 0x00;
	Memory::mem[Address::SCX] = 0x00;
	Memory::mem[Address::LYC] = 0x00;
	Memory::mem[Address::BGP] = 0xBF;
	Memory::mem[Address::OP0] = 0xFF;
	Memory::mem[Address::OP1] = 0xFF;
	Memory::mem[Address::WY] = 0x00;
	Memory::mem[Address::WX] = 0x00;
	Memory::mem[Address::IE] = 0x00;
	Memory::mem[Address::NR10] = 0x80;
	Memory::mem[Address::NR11] = 0xBF;
	Memory::mem[Address::NR12] = 0xF3;
	Memory::mem[Address::NR14] = 0xBF;
 	Memory::mem[Address::NR21] = 0x3F;
	Memory::mem[Address::NR22] = 0x00;
	Memory::mem[Address::NR24] = 0xBF;
	Memory::mem[Address::NR30] = 0x7F;
	Memory::mem[Address::NR31] = 0xFF;
	Memory::mem[Address::NR32] = 0x9F;
	Memory::mem[Address::NR33] = 0xBF;
	Memory::mem[Address::NR41] = 0xFF;
	Memory::mem[Address::NR42] = 0xFF;
	Memory::mem[Address::NR43] = 0x00;
	Memory::mem[Address::NR50] = 0x77;
	Memory::mem[Address::NR51] = 0xF3;
	Memory::mem[Address::NR52] = 0xF1;
}

// responsible for reading a byte from a specific memory location
u8 Memory::ReadByte(u16 address)
{
	return mem[address];
} 

// responsible for reading a word from a specific memory location
u16 Memory::ReadWord(u16 address)
{
	return ((mem[address] << 8) | (mem[address + 1]));
}

// responsible for writing a byte to a specific memory location
void Memory::WriteByte(u16 address, u8 data)
{
	switch (address)
	{
		// disable writes to protected memory
		case Address::PROT_MEM_START ... Address::PROT_MEM_END: break;

		// disable writes to LY (should never happen...)
		case Address::LY: break;

		// read from the serial port (useful for blarggs cpu tests)
		case Address::SERIAL_CTRL:
			if (data == 0x81) printf("%c", ReadByte(Address::SERIAL)); 
			mem[address] = data;
		break;

		// if writing to work ram, write to echo ram
		case Address::WRAM_START ... Address::WRAM_END:
			mem[address] = data;
			mem[address + 2000] = data;
		break;

		// if writing to echo ram, write to work ram
		case Address::ERAM_START ... Address::ERAM_END:
			mem[address] = data;
			mem[address - 2000] = data;
		break;

		// everything should be ok to write to memory here...
		default: mem[address] = data; break;
	}
}

// responsible for writing a word to a specific memory location
void Memory::WriteWord(u16 address, Cpu::Register reg)
{
	mem[address] = reg.lo;
	mem[address + 1] = reg.hi;
}

// responsible for popping a u16 from the stack
u16 Memory::Pop(u16 address)
{
	u16 data = ReadWord(address);
	Cpu::sp.reg += 2;

	return data;
}

// responsible for pushing a u16 to the stack
void Memory::Push(u16 address, Cpu::Register reg)
{
	Cpu::sp.reg -= 1;
	mem[Cpu::sp.reg] = reg.lo;
	Cpu::sp.reg -= 1;
	mem[Cpu::sp.reg] = reg.hi;
}
