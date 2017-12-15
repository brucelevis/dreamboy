/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "includes/mbc.h"
#include "includes/mbc1.h"
#include "includes/memory.h"
#include "includes/rom.h"

void Mbc::Manage(u16 address, u8 data)
{
	switch(Rom::mbcType)
	{
		case 0x1 ... 0x3: Mbc1::Manage(address, data); break;
		default: break;
	}
}