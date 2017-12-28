/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "includes/log.h"
#include "includes/mbc.h"
#include "includes/mbc1.h"
//#include "includes/mbc2.h"
//#include "includes/mbc3.h"
//#include "includes/mbc5.h"
#include "includes/memory.h"
#include "includes/rom.h"

// definitions
#define MBC1 0x1 ... 0x3
#define MBC2 0x5 ... 0x6
#define MBC3 0x12 ... 0x13
#define MBC5 0x19 ... 0x1E

// init vars
const u16 Mbc::maxSize[0x9] =
{
	[0x0] = 0x2, [0x1] = 0x4, [0x2] = 0x8, [0x3] = 0x10, [0x4] = 0x20, [0x5] = 0x40,
	[0x6] = 0x80, [0x7] = 0x100, [0x8] = 0x200
};

// responsible for returning the rom banks maximum size
u16 Mbc::GetMaxBankSize()
{
	return (maxSize[Rom::romSize] - 0x1);
}

// responsible for managing rom banking
void Mbc::RomBanking(u16 address, u8 data)
{
	switch(Rom::mbcType)
	{
		case MBC1: Mbc1::RomBanking(address, data); break;
		//case MBC2: Mbc2::RomBanking(address, data); break;
		//case MBC3: Mbc3::RomBanking(address, data); break;
		//case MBC5: Mbc5::RomBanking(address, data); break;
		default: break;
	}
}

// responsible for managing banking
void Mbc::ManageBanking(u16 address, u8 data)
{
	switch(address)
	{
		// handle selecting ram bank/upper two bits of rom bank
		case 0x4000 ... 0x5FFF:
			switch(Rom::mbcType)
			{
				case MBC1: Mbc1::ManageSelection(data); break;
				//case MBC2: Mbc2::ManageSelection(data); break;
				//case MBC3: Mbc3::ManageSelection(data); break;
				//case MBC5: Mbc5::ManageSelection(data); break;
				default: break;
			}
		break;

		// handle rom/ram mode
		case 0x6000 ... 0x7FFF:
			switch(Rom::mbcType)
			{
				case MBC1: Mbc1::ManageMode(data); break;
				//case MBC2: Mbc2::ManageMode(data); break;
				//case MBC3: Mbc3::ManageMode(data); break;
				//case MBC5: Mbc5::ManageMode(data); break;
				default: break;
			}
		break;
	}
}

