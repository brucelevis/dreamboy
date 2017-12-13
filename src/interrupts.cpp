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
static InterruptType vblank  = {{.bit = 0, .address = 0x0040}};
static InterruptType stat = {{.bit = 1, .address = 0x0048}};
static InterruptType timer = {{.bit = 2, .address = 0x0050}};
static InterruptType serial = {{.bit = 3, .address = 0x0058}};
static InterruptType joypad = {{.bit = 4, .address = 0x0060}};
static InterruptType interruptList[5] = {vblank, stat, timer, serial, joypad};
bool Interrupts::ime = false;
u8 Interrupts::pendingCount = 0;

// responsible for initialisizing the interrupt system
void Interrupts::Init()
{
	ime = false;
	pendingCount = 0;
	Memory::WriteByte(Memory::Address::IF, 0x00);
}

// responsible for resetting a pending interrupt
void Interrupts::Reset(u8 id)
{
	u8 data = Memory::ReadByte(Memory::Address::IF);
	Bit::Clear(data, interruptList[id].bit);
	Memory::WriteByte(Memory::Address::IF, data);
}

// responsible for detecting if an interrupt has been requested
bool Interrupts::IsRequested(u8 id)
{
	u8 data = Memory::ReadByte(Memory::Address::IF);
	return Bit::Get(data, interruptList[id].bit);
}

// responsible for detecting if an interrupt has been enabled
bool Interrupts::IsEnabled(u8 id)
{
	u8 data = Memory::ReadByte(Memory::Address::IE);
	return Bit::Get(data, interruptList[id].bit);
}

// responsible for requesting an interrupt
void Interrupts::Request(u8 id)
{
	u8 data = Memory::ReadByte(Memory::Address::IF);
	Bit::Set(data, interruptList[id].bit);
	Memory::WriteByte(Memory::Address::IF, data);
}

// responsible for returning the id of the requested interrupt (if enabled)
u8 Interrupts::RequestedId()
{
	u8 id = -1;

	for (u8 i = 0; i < 5; i++)
	{
		if (IsRequested(i) && IsEnabled(i)) id = i; break;
	}

	return id;
}

// responsible for servicing an interrupt
void Interrupts::Service()
{
	u8 id = RequestedId();

	if (id >= 0 && ime)
	{
		Reset(id);
		Memory::Push(Cpu::pc);

		Cpu::pc.reg = interruptList[id].address;
		ime = false;
	}
}
