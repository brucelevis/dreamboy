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
#include "includes/memory.h"
#include "includes/timer.h"

// definitions
#define TIMA Memory::mem[Memory::Address::TIMA]
#define TAC Memory::ReadByte(Memory::Address::TAC)
#define TMA Memory::ReadByte(Memory::Address::TMA)
#define DIV Memory::mem[Memory::Address::DIV]

// init vars
int Timer::timerCounter = 0;
int Timer::divCounter = 0;
static const u16 frequencies[4] = {1024, 16, 64, 256};

// responsible for initializing the timer
void Timer::Init()
{
	timerCounter = 0;
	divCounter = 0;
}

// responsible for getting the current frequency
u16 Timer::GetFrequency()
{
	return frequencies[TAC & 0x3];
}

// responsible for determining if the timer is enabled
bool Timer::Enabled()
{
	return Bit::Get(TAC, 2);
}

// responsible for updating div
void Timer::UpdateDiv(int cycles)
{
	divCounter += cycles;

	if (divCounter > 255)
	{
		DIV += 1;
		divCounter -= 256;
	}
}

// responsible for updating the timer
void Timer::Update(int cycles)
{
	UpdateDiv(cycles);

	if (!Enabled()) return;

	const u16 currentFrequency = GetFrequency();
	timerCounter += cycles;

	if (timerCounter >= currentFrequency)
	{
		if (((u16)TIMA + 1) > 255)
		{
			TIMA = TMA;
			Interrupts::Request(Interrupts::TIMER);
		}

		TIMA += 1;

		timerCounter -= currentFrequency;
	}
}
