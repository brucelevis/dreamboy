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
#include "includes/cpu.h"
#include "includes/interrupts.h"
#include "includes/log.h"
#include "includes/memory.h"

// definitions
#define IF Memory::mem[Memory::Address::IF]
#define IE Memory::mem[Memory::Address::IE]

// init vars
union InterruptType
{
	struct
	{
		u8 bit;
		u16 address;
	};
};
static InterruptType vblank  = {{.bit = 0, .address = 0x40}};
static InterruptType lcd = {{.bit = 1, .address = 0x48}};
static InterruptType timer = {{.bit = 2, .address = 0x50}};
static InterruptType serial = {{.bit = 3, .address = 0x58}};
static InterruptType joypad = {{.bit = 4, .address = 0x60}};
static InterruptType interruptList[5] = {vblank, lcd, timer, serial, joypad};
bool Interrupts::ime = false;
bool Interrupts::clearIF = true;
bool Interrupts::shouldExecute = true;
u8 Interrupts::pendingCount = 0;
static bool wasHalted = false;

// responsible for initialisizing the interrupt system
void Interrupts::Init()
{
	IF = 0xE1;
	IE = 0x00;
	ime = false;
	pendingCount = 0;
	clearIF = true;
	shouldExecute = true;
}

// responsible for resetting a pending interrupt
void Interrupts::Reset(int id)
{
	if (!clearIF) return;
	Bit::Clear(IF, interruptList[id].bit);
}

// responsible for detecting if an interrupt has been requested
bool Interrupts::IsRequested(int id)
{
	return Bit::Get(IF, interruptList[id].bit);
}

// responsible for detecting if an interrupt has been enabled
bool Interrupts::IsEnabled(int id)
{
	return Bit::Get(IE, interruptList[id].bit);
}

// responsible for requesting an interrupt
void Interrupts::Request(int id)
{
	Bit::Set(IF, interruptList[id].bit);
	IF |= 0xE0;
}

// responsible for returning the id of the requested interrupt (if enabled)
int Interrupts::RequestedId()
{
	for (int i = 0; i < 5; i++)
	{
		if (IsRequested(i) && IsEnabled(i))
		{
			wasHalted = Cpu::halted;
			Cpu::halted = false;
			return i;
		}
	}

	return -1;
}

// responsible for servicing an interrupt
void Interrupts::Service()
{
	int id = RequestedId();

	if (id >= 0 && ime)
	{
		if (shouldExecute)
		{
			Reset(id);
			Memory::Push(Cpu::pc);

			Cpu::cycles += (wasHalted) ? 24 : 20;
			Cpu::pc.reg = interruptList[id].address;
			wasHalted = false;
			ime = false;
		}
	}
}
