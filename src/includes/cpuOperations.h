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
	static void Add8(u8 &val, u8 add, int cycles);
	static void Adc8(u8 &val, u8 add, int cycles);
	static void Sub8(u8 &val, u8 sub, int cycles);
	static void Sbc8(u8 &val, u8 sub, int cycles);
	static void Inc8(u8 &val, int cycles);
	static void Dec8(u8 &val, int cycles);
	static void Cmp8(u8 &val, u8 compare, int cycles);
	static void Load8(u8 &val, u8 load, int cycles);
	static void Add16(u16 &val, u16 add, int cycles);
	static void Sub16(u16 &val, u16 sub, int cycles);
	static void Inc16(u16 &val, int cycles);
	static void Dec16(u16 &val, int cycles);
	static void Load16(u16 &val, u16 load, int cycles);
	static void JmpRel(bool condition, int cycles);
	static void JmpImm(bool condition, int cycles);
	static void Call(bool condition, int cycles);
	static void Ret(bool condition, int cycles);
	static void Rst(int cycles);
};

#endif
