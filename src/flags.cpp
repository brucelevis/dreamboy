/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "includes/cpu.h"
#include "includes/flags.h"

// definitions
#define F Cpu::af.lo

// init vars
const u8 Flags::z = 0x80;
const u8 Flags::n = 0x40;
const u8 Flags::h = 0x20;
const u8 Flags::c = 0x10;
const u8 Flags::all = (Flags::z  | Flags::n | Flags::h | Flags::c);

// responsible for getting a flags value
u8 Flags::Get(u8 flag)
{
	return (F & flag) ? 1 : 0;
}

// responsible for setting a flag
void Flags::Set(u8 flags)
{
	F |= flags;
}

// responsible for clearing a flag
void Flags::Clear(u8 flags)
{
	F &= ~flags;
}
