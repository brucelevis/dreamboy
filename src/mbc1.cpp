/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "includes/mbc1.h"
#include "includes/log.h"
#include "includes/memory.h"
#include "includes/rom.h"

// responsible for managing MBC1
void Mbc1::Manage(u16 address, u8 data)
{
	int bankNo = (data * 0x4000);
	memcpy(&Memory::mem[0x4000], &Rom::rom[bankNo], 0x4000);
}