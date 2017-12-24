/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef MBC1_H
#define MBC1_H

// includes
#include "typedefs.h"

class Mbc1
{
	public:
		static void RomBanking(u16 address, u8 data);
		static void ManageSelection(u8 data);
		static void ManageMode(u8 data);
};

#endif
