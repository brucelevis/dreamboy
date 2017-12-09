/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef BIT_H
#define BIT_H

// includes
#include "typedefs.h"

class Bit 
{
	public:
		static void Set(u8 &in, u8 bit);
		static void Clear(u8 &in, u8 bit);
		static u8 Get(u8 in, u8 bit);
		static u8 DidHalfCarry(u8 in, u8 add, u8 mask);
		static u8 DidHalfCarry(u16 in, u16 add, u16 mask);
		static u8 DidCarry(u16 in, u16 mask);
		static u8 DidCarry(int in, u16 mask);
};

#endif
