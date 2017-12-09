/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "src/includes/bit.h"
#include "src/includes/cpu.h"
#include "src/includes/cpuOperations.h"
#include "src/includes/flags.h"
#include "src/includes/memory.h"


// # 8 bit operations # //

void CpuOps::Add8(u8 &in, u8 val, int cycles)
{
	u8 result = (in + val);

	Flags::Clear(Flags::all);

	if (result == 0) Flags::Set(Flags::z);
	if (Bit::DidHalfCarry(in, val, 0xF)) Flags::Set(Flags::h);
	if (Bit::DidCarry((u16)(in + val), 0xFF)) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Adc8(u8 &in, u8 val, int cycles)
{
	u8 carry = Flags::Get(Flags::c);
	u8 result = (in + carry + val);

	Flags::Clear(Flags::all);

	if (result == 0) Flags::Set(Flags::z);
	if (Bit::DidHalfCarry(in, carry, 0xF)) Flags::Set(Flags::h);
	if (Bit::DidHalfCarry(in + carry, val, 0xF)) Flags::Set(Flags::h);
	if (Bit::DidCarry((u16)(in + carry), 0xFF)) Flags::Set(Flags::c);
	if (Bit::DidCarry((u16)(in + carry + val), 0xFF)) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Sub8(u8 &in, u8 val, int cycles)
{
	u8 result = (in - val);

	Flags::Clear(Flags::z | Flags::h | Flags::c);
	Flags::Set(Flags::n);

	if (result == 0) Flags::Set(Flags::z);
	if ((in & 0xF) < (val & 0xF)) Flags::Set(Flags::h);
	if (in < val) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Sbc8(u8 &in, u8 sub, int cycles)
{
	u8 carry = Flags::Get(Flags::c);
	u8 result = (in - carry - sub);

	Flags::Clear(Flags::z | Flags::h | Flags::c);
	Flags::Set(Flags::n);

	if (result == 0) Flags::Set(Flags::z);
	if ((in & 0xF) < (carry & 0xF)) Flags::Set(Flags::h);
	if (((in - carry) & 0xF) < (sub & 0xF)) Flags::Set(Flags::h);
	if (in < carry) Flags::Set(Flags::c);
	if ((in - carry) < sub) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles +=cycles;
}

void CpuOps::Inc8(u8 &in, int cycles)
{
	u8 result = (in + 1);

	Flags::Clear(Flags::z | Flags::n | Flags::h);

	if (result == 0) Flags::Set(Flags::z);
	if ((result & 0xF) == 0) Flags::Set(Flags::h);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Inc8Mem(u16 address, int cycles)
{
	u8 data = Memory::ReadByte(address);

	Inc8(data, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Dec8(u8 &in, int cycles)
{
	u8 result = (in - 1);

	Flags::Clear(Flags::z | Flags::h);
	Flags::Set(Flags::n);

	if (result == 0) Flags::Set(Flags::z);
	if ((result & 0xF) == 0xF) Flags::Set(Flags::h);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Dec8Mem(u16 address, int cycles)
{
	u8 data = Memory::ReadByte(address);

	Dec8(data, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Cmp8(u8 &in, u8 compare, int cycles)
{
	u8 result = (in - compare);

	Flags::Clear(Flags::z | Flags::h | Flags::c);
	Flags::Set(Flags::n);

	if (result == 0) Flags::Set(Flags::z);
	if ((in & 0xF) < (compare & 0xF)) Flags::Set(Flags::h);
	if (in < compare) Flags::Set(Flags::c);

	Cpu::cycles +=cycles;
}

void CpuOps::And8(u8 &in, u8 val, int cycles)
{
	u8 result = (in & val);

	Flags::Clear(Flags::z | Flags::n | Flags::c);
	Flags::Set(Flags::h);

	if (result == 0) Flags::Set(Flags::z);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Or8(u8 &in, u8 val, int cycles)
{
	u8 result = (in | val);

	Flags::Clear(Flags::all);

	if (result == 0) Flags::Set(Flags::z);

	in = result;
	Cpu::cycles +=cycles;
}

void CpuOps::Xor8(u8 &in, u8 val, int cycles)
{
	u8 result = (in ^ val);

	Flags::Clear(Flags::all);

	if (result == 0) Flags::Set(Flags::z);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Rlc8(u8 &in, bool checkZero,  int cycles)
{
	u8 result = ((in << 1) | (in >> 7));

	Flags::Clear(Flags::all);

	if (checkZero && (result == 0)) Flags::Set(Flags::z);
	if (Bit::Get(in, 7)) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Rlc8Mem(u16 address, bool checkZero,  int cycles)
{
	u8 data = Memory::ReadByte(address);

	Rlc8(data, checkZero, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Rrc8(u8 &in, bool checkZero, int cycles)
{
	u8 result = ((in << 1) | (in << 7));

	Flags::Clear(Flags::all);

	if (checkZero && (result == 0)) Flags::Set(Flags::z);
	if (Bit::Get(in, 0)) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Rrc8Mem(u16 address, bool checkZero, int cycles)
{
	u8 data = Memory::ReadByte(address);

	Rrc8(data, checkZero, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Rl8(u8 &in, bool checkZero, int cycles)
{
	u8 carry = Flags::Get(Flags::c);
	u8 result = ((in << 1) | carry);

	Flags::Clear(Flags::all);

	if (checkZero && (result == 0)) Flags::Set(Flags::z);
	if (Bit::Get(in, 7)) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Rl8Mem(u16 address, bool checkZero, int cycles)
{
	u8 data = Memory::ReadByte(address);

	Rl8(data, checkZero, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Rr8(u8 &in, bool checkZero, int cycles)
{
	u8 carry = Flags::Get(Flags::c);
	u8 result = ((in >> 1) | carry);

	Flags::Clear(Flags::all);

	if (checkZero && (result == 0)) Flags::Set(Flags::z);
	if (Bit::Get(in, 0)) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Rr8Mem(u16 address, bool checkZero, int cycles)
{
	u8 data = Memory::ReadByte(address);

	Rr8(data, checkZero, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Slc8(u8 &in, int cycles)
{
	u8 result = (in << 1);

	Flags::Clear(Flags::all);

	if (result == 0) Flags::Set(Flags::z);
	if (Bit::Get(in, 7)) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Slc8Mem(u16 address, int cycles)
{
	u8 data = Memory::ReadByte(address);

	Slc8(data, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Src8(u8 &in, int cycles)
{
	u8 result = (in >> 1);
	u8 oldMsb = Bit::Get(in, 7);

	Flags::Clear(Flags::all);

	if (result == 0) Flags::Set(Flags::z);
	if (Bit::Get(in, 0)) Flags::Set(Flags::c);

	in = result;

	if (oldMsb) Bit::Set(in, 7); else Bit::Clear(in, 7);

	Cpu::cycles += cycles;
}

void CpuOps::Src8Mem(u16 address, int cycles)
{
	u8 data = Memory::ReadByte(address);

	Src8(data, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Daa(int cycles)
{
	Cpu::cycles += cycles;
}

void CpuOps::CmplA(int cycles)
{
	Flags::Set(Flags::n | Flags::h);

	Cpu::af.hi ^= 0xFF;
	Cpu::cycles += cycles;
}

void CpuOps::Scf(int cycles)
{
	Flags::Clear(Flags::n | Flags::h);
	Flags::Set(Flags::c);

	Cpu::cycles += cycles;
}

void CpuOps::Ccf(int cycles)
{
	Flags::Clear(Flags::n | Flags::h);

	if (Flags::Get(Flags::c)) Flags::Clear(Flags::c); else Flags::Set(Flags::c);

	Cpu::cycles += cycles;
}

void CpuOps::BitTest(u8 in, u8 bit, int cycles)
{
	Flags::Clear(Flags::z | Flags::n);
	Flags::Set(Flags::h);

	if (!Bit::Get(in, bit)) Flags::Set(Flags::z);

	Cpu::cycles += cycles;
}

void CpuOps::BitTestMem(u16 address, u8 bit, int cycles)
{
	u8 data = Memory::ReadByte(address);
	BitTest(data, bit, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::BitSet(u8 &in, u8 bit, int cycles)
{
	Bit::Set(in, bit);
	Cpu::cycles += cycles;
}

void CpuOps::BitSetMem(u16 address, u8 bit, int cycles)
{
	u8 data = Memory::ReadByte(address);
	BitSet(data, bit, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::BitClear(u8 &in, u8 bit, int cycles)
{
	Bit::Clear(in, bit);
	Cpu::cycles += cycles;
}

void CpuOps::BitClearMem(u16 address, u8 bit, int cycles)
{
	u8 data = Memory::ReadByte(address);
	BitClear(data, bit, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::BitSwap(u8 &in, int cycles)
{
	u8 result = (((in & 0xF0) >> 4) | ((in << 0x0F) << 4));

	Flags::Clear(Flags::all);

	if (result == 0) Flags::Set(Flags::z);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::BitSwapMem(u16 address, int cycles)
{
	u8 data = Memory::ReadByte(address);

	BitSwap(data, cycles);
	Memory::WriteByte(address, data);
}

void CpuOps::Load8(u8 &in, u8 val, int cycles)
{
	in = val;
	Cpu::cycles += cycles;
}

void CpuOps::Write8(u16 address, u8 data, int cycles)
{
	Memory::WriteByte(address, data);
	Cpu::cycles += cycles;
}

// # 16 bit operations # //

void CpuOps::Add16(u16 &in, u16 val, int cycles)
{
	u16 result = (in + val);

	Flags::Clear(Flags::n | Flags::h | Flags::c);

	if (Bit::DidHalfCarry(in, val, 0xFFF)) Flags::Set(Flags::h);
	if (Bit::DidCarry((int)(in + val), 0xFFFF)) Flags::Set(Flags::c);

	in = result;
	Cpu::cycles += cycles;
}

void CpuOps::Inc16(u16 &in, int cycles)
{
	in += 1;
	Cpu::cycles += cycles;
}

void CpuOps::Dec16(u16 &in, int cycles)
{
	in -= 1;
	Cpu::cycles += cycles;
}

void CpuOps::AddSpR8(s8 r8, int cycles)
{
	u16 result = (Cpu::sp.reg + r8);

	Flags::Clear(Flags::all);

	if (Bit::DidHalfCarry(Cpu::sp.reg, r8, 0xF)) Flags::Set(Flags::h);
	if (Bit::DidCarry(Cpu::sp.reg + r8, 0xFF)) Flags::Set(Flags::c);

	Cpu::sp.reg = result;
	Cpu::cycles += cycles;
}

void CpuOps::Load16(u16 &in, u16 val, int cycles)
{
	in = val;
	Cpu::cycles += cycles;
}

void CpuOps::LoadHlSpR8(s8 r8, int cycles)
{
	u16 result = (Cpu::sp.reg + r8);

	Flags::Clear(Flags::all);

	if (Bit::DidHalfCarry(Cpu::hl.reg, result, 0xF)) Flags::Set(Flags::h);
	if (Bit::DidCarry(Cpu::hl.reg + result, 0xFF)) Flags::Set(Flags::c);

	Cpu::hl.reg = result;
	Cpu::cycles += cycles;
}

// # flow control # //

void CpuOps::JmpRel(bool condition, int cycles)
{
	s8 r8 = (s8)Memory::ReadByte(Cpu::pc.reg);

	if (condition)
	{
		Cpu::pc.reg += r8;
		Cpu::cycles += 4;
	}

	Cpu::pc.reg += 1;
	Cpu::cycles += cycles;
}

void CpuOps::JmpImm(bool condition, int cycles)
{
	if (condition)
	{
		Cpu::pc.reg = Memory::ReadWord(Cpu::pc.reg);
		Cpu::cycles += (cycles + 4);
		return;
	}

	Cpu::pc.reg += 2;
	Cpu::cycles += cycles;
}

void CpuOps::Call(bool condition, int cycles)
{
	if (condition)
	{
		Cpu::pc.reg += 2;
		Memory::Push(Cpu::pc);
		Cpu::pc.reg = Memory::ReadWord(Cpu::pc.reg -= 2);
		Cpu::cycles += (cycles + 12);
		return;
	}

	Cpu::pc.reg += 2;
	Cpu::cycles += cycles;
}

void CpuOps::Ret(bool condition, int cycles)
{
	if (condition)
	{
		Cpu::pc.reg = Memory::Pop();
		Cpu::cycles += 12;
	}

	Cpu::cycles += cycles;
}

void CpuOps::Rst(u16 address, int cycles)
{
	Memory::Push(Cpu::pc);

	Cpu::pc.reg = address;
	Cpu::cycles += cycles;
}

// # misc # //

void CpuOps::Nop(int cycles)
{
	Cpu::cycles += cycles;
}

void CpuOps::Stop(int cycles)
{
	Cpu::stopped = true;
	Cpu::cycles += cycles;
}

void CpuOps::Halt(int cycles)
{
	Cpu::halted = true;
	Cpu::cycles += cycles;
}

void CpuOps::DI(int cycles)
{
	Cpu::cycles += cycles;
}

void CpuOps::EI(int cycles)
{
	Cpu::cycles += cycles;
}
