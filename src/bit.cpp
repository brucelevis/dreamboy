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

// responsible for setting a bit
void Bit::Set(u8 &in, u8 bit)
{
	u8 mask = (1 << bit);
	in |= mask;
}

// responsible for clearing a bit
void Bit::Clear(u8 &in, u8 bit)
{
	u8 mask = (1 << bit);
	in &= ~mask;
}

// responsible for getting a bits value
u8 Bit::Get(u8 in, u8 bit)
{
	u8 mask = (1 << bit);
	return (in & mask) ? 1 : 0;
}

// responsible for determining if we half-carried
u8 Bit::DidHalfCarry(u8 in, u8 add, u8 mask)
{
	return (((in & mask) + (add & mask)) > mask);
}

// responsible for determining if we half-carried
u8 Bit::DidHalfCarry(u16 val, u16 val2, u16 mask)
{
	return (((val & mask) + (val2 & mask)) > mask);
}

// responsible for determining if we carried
u8 Bit::DidCarry(u16 val, u16 mask)
{
	return (val > mask);
}

// responsible for determining if we carried
u8 Bit::DidCarry(int val, u16 mask)
{
	return (val > mask);
}
