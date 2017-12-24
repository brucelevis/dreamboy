/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef MBC_H
#define MBC_H

// includes
#include "typedefs.h"

class Mbc
{
	public:
		static u8 GetMaxBankSize();
		static void RomBanking(u16 address, u8 data);
		static void ManageBanking(u16 address, u8 data);

	private:
		static const u8 maxSize[0x55];
};

#endif
