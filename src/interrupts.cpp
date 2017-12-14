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
u8 Interrupts::pendingCount = 0;
static bool wasHalted = false;

// responsible for initialisizing the interrupt system
void Interrupts::Init()
{
	ime = false;
	pendingCount = 0;
	Memory::WriteByte(Memory::Address::IF, 0xE1);
	Memory::WriteByte(Memory::Address::IE, 0x00);
}

// responsible for resetting a pending interrupt
void Interrupts::Reset(int id)
{
	u8 data = Memory::ReadByte(Memory::Address::IF);
	Bit::Clear(data, interruptList[id].bit);
	Memory::WriteByte(Memory::Address::IF, (data & 0x1F));
}

// responsible for detecting if an interrupt has been requested
bool Interrupts::IsRequested(int id)
{
	u8 data = Memory::ReadByte(Memory::Address::IF);
	return Bit::Get(data, interruptList[id].bit);
}

// responsible for detecting if an interrupt has been enabled
bool Interrupts::IsEnabled(int id)
{
	u8 data = Memory::ReadByte(Memory::Address::IE);
	return Bit::Get(data, interruptList[id].bit);
}

// responsible for requesting an interrupt
void Interrupts::Request(int id)
{
	u8 data = Memory::ReadByte(Memory::Address::IF);
	Bit::Set(data, interruptList[id].bit);
	Memory::WriteByte(Memory::Address::IF, (data & 0x1F));
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
		Reset(id);
		Memory::Push(Cpu::pc);

		Cpu::pc.reg = interruptList[id].address;
		wasHalted = false;
		ime = false;
	}
}
