/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "includes/bit.h"
#include "includes/input.h"
#include "includes/log.h"
#include "includes/mbc.h"
#include "includes/memory.h"
#include "includes/rom.h"

// init vars
u8 Memory::mem[0x10000] = {0x00};
bool Memory::useRomBank = true;
bool Memory::useRamBank = false;

// responsible for initializing the memory
void Memory:: Init()
{
	memset(mem, 0x00, sizeof(mem));

	mem[Address::DIV] = 0xAB;
	mem[Address::TIMA] = 0x00;
	mem[Address::TMA] = 0x00;
	mem[Address::TAC] = 0xF8;
	mem[Address::LCDC] = 0x91;
	mem[Address::STAT] = 0x85;
	mem[Address::SCY] = 0x00;
	mem[Address::SCX] = 0x00;
	mem[Address::LYC] = 0x00;
	mem[Address::BGP] = 0xBF;
	mem[Address::OP0] = 0xFF;
	mem[Address::OP1] = 0xFF;
	mem[Address::WY] = 0x00;
	mem[Address::WX] = 0x00;
	mem[Address::P1] = 0xFF;
	mem[Address::IF] = 0xE1;
	mem[Address::IE] = 0x00;
	mem[Address::NR10] = 0x80;
	mem[Address::NR11] = 0xBF;
	mem[Address::NR12] = 0xF3;
	mem[Address::NR14] = 0xBF;
 	mem[Address::NR21] = 0x3F;
	mem[Address::NR22] = 0x00;
	mem[Address::NR24] = 0xBF;
	mem[Address::NR30] = 0x7F;
	mem[Address::NR31] = 0xFF;
	mem[Address::NR32] = 0x9F;
	mem[Address::NR33] = 0xBF;
	mem[Address::NR41] = 0xFF;
	mem[Address::NR42] = 0xFF;
	mem[Address::NR43] = 0x00;
	mem[Address::NR50] = 0x77;
	mem[Address::NR51] = 0xF3;
	mem[Address::NR52] = 0xF1;
}

// responsible for reading a byte from a specific memory location
u8 Memory::ReadByte(u16 address)
{
	switch(address)
	{
		case Address::ROM_BK1_START ... Address::ROM_BK1_END:
		{
			int bankAddr = ((Rom::romBank * 0x4000) + (address - 0x4000));
			if (!useRomBank) bankAddr = (((Rom::romBank & Mbc::GetMaxBankSize()) * 0x4000) + (address - 0x4000));
			return Rom::rom[bankAddr];
		}
		break;
		case Address::EXTRAM_START ... Address::EXTRAM_END:
			if (useRamBank)
			{
				const u8 ramBank = (Rom::currentMode == 0x0) ? 0x0 : Rom::ramBank;
				return Rom::ram[(ramBank * 0x2000) + (address - Address::EXTRAM_START)];
			}
			else
			{
				return 0xFF;
			}
		break;
		case Address::P1: return Input::GetKey(mem[address]); break;
		case Address::NR10: return 0xFF; break;
		case Address::NR11: return 0xFF; break;
		case Address::NR12: return 0xFF; break;
		case Address::NR14: return 0xFF; break;
		case Address::NR21: return 0xFF; break;
		case Address::NR22: return 0xFF; break;
		case Address::NR24: return 0xFF; break;
		case Address::NR30: return 0xFF; break;
		case Address::NR31: return 0xFF; break;
		case Address::NR32: return 0xFF; break;
		case Address::NR33: return 0xFF; break;
		case Address::NR41: return 0xFF; break;
		case Address::NR42: return 0xFF; break;
		case Address::NR43: return 0xFF; break;
		case Address::NR50: return 0xFF; break;
		case Address::NR51: return 0xFF; break;
		case Address::NR52: return 0xFF; break;
		default: break;
	}

	return mem[address];
}

// responsible for reading a word from a specific memory location
u16 Memory::ReadWord(u16 address)
{
	if (address >= Address::ROM_BK1_START && address <= Address::ROM_BK1_END)
	{
		int bankAddr = ((Rom::romBank * 0x4000) + (address - 0x4000));
		if (!useRomBank) bankAddr = (((Rom::romBank & Mbc::GetMaxBankSize()) * 0x4000) + (address - 0x4000));
		return ((Rom::rom[bankAddr + 1] << 8) | (Rom::rom[bankAddr]));
	}

	return ((mem[address + 1] << 8) | (mem[address]));
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

		// handle DMA writes
		case Address::DMA:
		{
			const u16 addr = (data << 8);

			for (u16 i = 0; i < 0xA0; i++)
			{
				mem[0xFE00 + i] = ReadByte(addr + i);
			}
		}
		break;

		// reset DIV if it is written to
		case Address::DIV: mem[address] = 0x00; break;

		// read from the serial port (useful for blarggs cpu tests)
		case Address::SERIAL_CTRL:
			if (data == 0x81) printf("%c", ReadByte(Address::SERIAL));
			mem[address] = data;
		break;

		// if writing to work ram, write to echo ram
		case Address::WRAM_START ... Address::WRAM_END:
			mem[address] = data;
			mem[address + 0x2000] = data;
		break;

		// if writing to echo ram, write to work ram
		case Address::ERAM_START ... Address::ERAM_END:
			mem[address] = data;
			mem[address - 0x2000] = data;
		break;

		// if writing specific data to unmapped memory
		case Address::UNMAPPED_START ... Address::UNMAPPED_END:
			// copy rom back over bios
			if (data == 0x1) Rom::Reload();
		break;

		// handle enabling ram banking
		case 0x0000 ... 0x1FFF:
			useRamBank = ((data & 0xF) == 0xA) ? true : false;
		break;

		// rom banking
		case 0x2000 ... 0x3FFF: Mbc::RomBanking(address, data); break;

		// manage rom banking
		case 0x4000 ... 0x7FFF: Mbc::ManageBanking(address, data); break;

		// handle external ram
		case Address::EXTRAM_START ... Address::EXTRAM_END:
			if (useRamBank)
			{
				Rom::ram[(Rom::ramBank * 0x2000) + (address - Address::EXTRAM_START)] = data;
			}
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
u16 Memory::Pop()
{
	const u16 data = ReadWord(Cpu::sp.reg);
	Cpu::sp.reg += 2;

	return data;
}

// responsible for pushing a u16 to the stack
void Memory::Push(Cpu::Register reg)
{
	Cpu::sp.reg -= 1;
	mem[Cpu::sp.reg] = reg.hi;
	Cpu::sp.reg -= 1;
	mem[Cpu::sp.reg] = reg.lo;
}
