/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef MEMORY_H
#define MEMORY_H

// includes
#include "typedefs.h"
#include "cpu.h"

class Memory
{
	public:
		static void Init();
		static u8 ReadByte(u16 address);
		static u16 ReadWord(u16 address);
		static void WriteByte(u16 address, u8 data);
		static void WriteWord(u16 address, Cpu::Register reg);
		static u16 Pop();
		static void Push(Cpu::Register reg);

	public:
		static u8 mem[0x10000];

	public:
		class Address
		{
			public:
				static const u16 LCDC = 0xFF40;
				static const u16 STAT = 0xFF41;
				static const u16 SCY = 0xFF42;
				static const u16 SCX = 0xFF43;
				static const u16 WY = 0xFF4A;
				static const u16 WX = 0xFF4B;
				static const u16 LY = 0xFF44;
				static const u16 LYC = 0xFF45;
				static const u16 BGP = 0xFF47;
				static const u16 OP0 = 0xFF48;
				static const u16 OP1 = 0xFF49;
				static const u16 DMA = 0xFF46;
				static const u16 P1 = 0xFF00;
				static const u16 SERIAL = 0xFF01;
				static const u16 SERIAL_CTRL = 0xFF02;
				static const u16 DIV = 0xFF04;
				static const u16 TIMA = 0xFF05;
				static const u16 TMA = 0xFF06;
				static const u16 TAC = 0xFF07;
				static const u16 IE = 0xFFFF;
				static const u16 IF = 0xFF0F;
				static const u16 NR10 = 0xFF10;
				static const u16 NR11 = 0xFF11;
				static const u16 NR12 = 0xFF12;
				static const u16 NR14 = 0xFF14;
				static const u16 NR21 = 0xFF16;
				static const u16 NR22 = 0xFF17;
				static const u16 NR24 = 0xFF19;
				static const u16 NR30 = 0xFF1A;
				static const u16 NR31= 0xFF1B;
				static const u16 NR32 = 0xFF1C;
				static const u16 NR33 = 0xFF1E;
				static const u16 NR41 = 0xFF20;
				static const u16 NR42 = 0xFF21;
				static const u16 NR43 = 0xFF22;
				static const u16 NR50 = 0xFF24;
				static const u16 NR51 = 0xFF25;
				static const u16 NR52 = 0xFF26;
				static const u16 WRAM_START = 0xC000;
				static const u16 WRAM_END = 0xDDFF;
				static const u16 ERAM_START = 0xE000;
				static const u16 ERAM_END = 0xFDFF;
				static const u16 PROT_MEM_START = 0xFEA0;
				static const u16 PROT_MEM_END = 0xFEFF;
				static const u16 HRAM_START = 0xFF80;
				static const u16 HRAM_END = 0xFFFE;
				static const u16 UNMAPPED_START = 0xFF4C;
				static const u16 UNMAPPED_END = 0xFF7F;
				static const u16 ROM_BK0_START = 0x0000;
				static const u16 ROM_BK0_END = 0x3FFF;
				static const u16 ROM_BK1_START = 0x4000;
				static const u16 ROM_BK1_END = 0x7FFF;
				static const u16 EXTRAM_START = 0xA000;
				static const u16 EXTRAM_END = 0xBFFF;
				static const u16 ROM_NAME_START = 0x0134;
				static const u16 ROM_NAME_END = 0x0143;
				static const u16 ROM_TYPE = 0x0147;
				static const u16 ROM_SIZE = 0x0148;
				static const u16 ROM_RAM_SIZE = 0x0149;
		};
};

#endif
