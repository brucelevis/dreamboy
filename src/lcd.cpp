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
#include "includes/interrupts.h"
#include "includes/lcd.h"
#include "includes/log.h"
#include "includes/memory.h"

// definitions
#define LCD_CLOCK_CYCLES 456
#define LY Memory::mem[Memory::Address::LY]
#define LCDC Memory::mem[Memory::Address::LCDC]

// init vars
int Lcd::scanlineCounter = 0;

void Lcd::Init()
{
	scanlineCounter = 0;
}

void Lcd::Reset()
{

}

bool Lcd::Enabled()
{
	return Bit::Get(LCDC, 7);
}

void Lcd::Update(int cycles)
{
	if (!Enabled())
	{
		scanlineCounter  = 0;
		LY = 0;
		return;
	}

	scanlineCounter += cycles;

	if (scanlineCounter >= LCD_CLOCK_CYCLES)
	{
		LY += 1;

		if (LY == 144)
		{
			Interrupts::Request(Interrupts::ID::VBLANK);
		}
		else if (LY > 153)
		{
			LY = 0;
		}

		scanlineCounter -= LCD_CLOCK_CYCLES;
	}
}
