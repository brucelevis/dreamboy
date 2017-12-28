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
#include "includes/log.h"
#include "includes/mbc.h"
#include "includes/mbc1.h"
#include "includes/memory.h"
#include "includes/rom.h"

// responsible for managing MBC1 rom banking
void Mbc1::RomBanking(u16 address, u8 data)
{
	u8 bankNo = (data & 0x1F);

	if (bankNo == 0x00 || bankNo == 0x20 || bankNo == 0x40 || bankNo == 0x60)
	{
		bankNo += 0x1;
	}

	Rom::romBank = bankNo;
}

// responsible for managing the bank selection(s)
void Mbc1::ManageSelection(u8 data)
{
	// 0 = 16/8 mode || 1 = 4/32 mode
	if (Rom::currentMode == 0x0)
	{
		const u8 romBankMask = Mbc::GetMaxBankSize();
		Rom::romBank &= romBankMask;
		Rom::romBank |= (((data & 0x3) << 5) & romBankMask);
	}
	else
	{
		// only ram sizes 0x3 and 0x4 have more than one ram bank
		if (Rom::ramSize > 0x2) Rom::ramBank = (data & 0x3);
	}
}

// responsible for managing the bank mode(s)
void Mbc1::ManageMode(u8 data)
{
	Rom::currentMode = (data & 0x1);
	Memory::useRomBank = (Rom::currentMode == 0x0);
}
