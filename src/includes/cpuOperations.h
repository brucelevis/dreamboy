/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef CPU_OPS
#define CPU_OPS

// includes
#include "typedefs.h"

class CpuOps
{
	public:
		static void Add8(u8 &in, u8 val, int cycles);
		static void Adc8(u8 &in, u8 val, int cycles);
		static void Sub8(u8 &in, u8 val, int cycles);
		static void Sbc8(u8 &in, u8 val, int cycles);
		static void Inc8(u8 &in, int cycles);
		static void Inc8Mem(u16 address, int cycles);
		static void Dec8(u8 &in, int cycles);
		static void Dec8Mem(u16 address, int cycles);
		static void Cmp8(u8 &in, u8 compare, int cycles);
		static void And8(u8 &in, u8 val, int cycles);
		static void Or8(u8 &in, u8 val, int cycles);
		static void Xor8(u8 &in, u8 val, int cycles);
		static void Rlc8(u8 &in, int cycles);
		static void Rrc8(u8 &in, int cycles);
		static void Rl8(u8 &in, int cycles);
		static void Rr8(u8 &in, int cycles);
		static void Daa(int cycles);
		static void CmplA(int cycles);
		static void Scf(int cycles);
		static void Ccf(int cycles);
		static void BitTest(u8 in, u8 bit, int cycles);
		static void BitTestMem(u16 address, u8 bit, int cycles);
		static void BitSet(u8 &in, u8 bit, int cycles);
		static void BitSetMem(u16 address, u8 bit, int cycles);
		static void BitClear(u8 &in, u8 bit, int cycles);
		static void BitClearMem(u16 address, u8 bit, int cycles);
		static void Load8(u8 &in, u8 val, int cycles);
		static void Write8(u16 address, u8 data, int cycles);
		static void Add16(u16 &in, u16 val, int cycles);
		static void Inc16(u16 &in, int cycles);
		static void Dec16(u16 &in, int cycles);
		static void AddSpR8(s8 r8, int cycles);
		static void Load16(u16 &in, u16 val, int cycles);
		static void LoadHlSpR8(s8 r8, int cycles);
		static void JmpRel(bool condition, int cycles);
		static void JmpImm(bool condition, int cycles);
		static void Call(bool condition, int cycles);
		static void Ret(bool condition, int cycles);
		static void Rst(u16 address, int cycles);
		static void Nop(int cycles);
		static void Stop(int cycles);
		static void Halt(int cycles);
		static void DI(int cycles);
		static void EI(int cycles);
};

#endif
