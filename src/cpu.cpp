/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#include "src/includes/cpu.h"
#include "src/includes/cpuOperations.h"
#include "src/includes/flags.h"
#include "src/includes/log.h"
#include "src/includes/memory.h"

// definitions
#define A Cpu::af.hi
#define F Cpu::af.lo
#define B Cpu::bc.hi
#define C Cpu::bc.lo
#define D Cpu::de.hi
#define E Cpu::de.lo
#define H Cpu::hl.hi
#define L Cpu::hl.lo
#define AF Cpu::af.reg
#define BC Cpu::bc.reg
#define DE Cpu::de.reg
#define HL Cpu::hl.reg
#define SP Cpu::sp.reg
#define PC Cpu::pc.reg

// init vars
Cpu::Register Cpu::af = {};
Cpu::Register Cpu::bc = {};
Cpu::Register Cpu::de = {};
Cpu::Register Cpu::hl = {};
Cpu::Register Cpu::sp = {};
Cpu::Register Cpu::pc = {};
int Cpu::cycles = 0;
int Cpu::instructionsRan = 0;
bool Cpu::halted = false;
bool Cpu::stopped = false;
bool Cpu::pendingInterrupt = false;
bool Cpu::didLoadBios = false;

// responsible for initializing the Cpu
void Cpu::Init()
{
	if (didLoadBios)
	{
		af.reg = 0x0000;
		bc.reg = 0x0000;
		de.reg = 0x0000;
		hl.reg = 0x0000;
		sp.reg = 0x0000;
		pc.reg = 0x0000;
	}
	else
	{
		af.reg = 0x01B0;
		bc.reg = 0x0013;
		de.reg = 0x00D8;
		hl.reg = 0x014D;
		sp.reg = 0xFFFE;
		pc.reg = 0x100;
	}

	cycles = 0;
	instructionsRan = 0;
	halted = false;
	stopped = false;
	pendingInterrupt = false;
}

// responsible for executing the current opcode
void Cpu::ExecuteOpcode()
{
	u8 opcode = Memory::ReadByte(PC);
	instructionsRan += 1;

	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "%04X:%04X:%04X:%04X:%04X:%04X:%04X\n", PC, opcode, AF, BC, DE, HL, SP);
	Log::ToFile(buffer);

	// TODO: Handle stop/halt
	PC += 1;

	switch(opcode)
	{
		case 0x00: CpuOps::Nop(4); break; // NOP
		case 0x01: CpuOps::Load16(BC, Memory::ReadWord(PC), 12); PC += 2; break; // LD BC,d16
		case 0x02: CpuOps::Write8(BC, A, 8); break; // LD (BC),A
		case 0x03: CpuOps::Inc16(BC, 8); break; // INC BC
		case 0x04: CpuOps::Inc8(B, 4); break; // INC B
		case 0x05: CpuOps::Dec8(B, 4); break; // DEC B
		case 0x06: CpuOps::Load8(B, Memory::ReadByte(PC), 8); PC += 1; break; // LD B,d8
		case 0x07: CpuOps::Rlc8(A, false, 4); break; // RLCA
		case 0x08: Memory::WriteWord(Memory::ReadWord(PC), sp); PC += 2; cycles += 20; break; // LD (a16),SP
		case 0x09: CpuOps::Add16(HL, BC, 8); break; // ADD HL,BC
		case 0x0A: CpuOps::Load8(A, Memory::ReadByte(BC), 8); break; // LD A,(BC)
		case 0x0B: CpuOps::Dec16(BC, 8); break; // DEC BC
		case 0x0C: CpuOps::Inc8(C, 4); break; // INC C
		case 0x0D: CpuOps::Dec8(C, 4); break; // DEC C
		case 0x0E: CpuOps::Load8(C, Memory::ReadByte(PC), 8); PC += 1; break; // LD C,d8
		case 0x0F: CpuOps::Rrc8(A, false, 4); break; // RRCA
		case 0x10: CpuOps::Stop(4); break; // STOP
		case 0x11: CpuOps::Load16(DE, Memory::ReadWord(PC), 12); PC += 2; break; // LD DE,d16
		case 0x12: CpuOps::Write8(DE, A, 8); break; // LD (DE),A
		case 0x13: CpuOps::Inc16(DE, 8); break; // INC DE
		case 0x14: CpuOps::Inc8(D, 4); break; // INC D
		case 0x15: CpuOps::Dec8(D, 4); break; // DEC D
		case 0x16: CpuOps::Load8(D, Memory::ReadByte(PC), 8); PC += 1; break; // LD D,d8
		case 0x17: CpuOps::Rl8(A, false, 4); break; // RLA
		case 0x18: CpuOps::JmpRel(true, 8); break; // JR r8
		case 0x19: CpuOps::Add16(HL, DE, 8); break; // ADD HL,DE
		case 0x1A: CpuOps::Load8(A, Memory::ReadByte(DE), 8); break; // LD A,(DE)
		case 0x1B: CpuOps::Dec16(DE, 8); break; // DEC DE
		case 0x1C: CpuOps::Inc8(E, 4); break; // INC E
		case 0x1D: CpuOps::Dec8(E, 4); break; // DEC E
		case 0x1E: CpuOps::Load8(E, Memory::ReadByte(PC), 8); PC += 1; break; // LD E,d8
		case 0x1F: CpuOps::Rr8(A, false, 4); break; // RRA
		case 0x20: CpuOps::JmpRel(!Flags::Get(Flags::z), 8); break; // JR NZ,r8
		case 0x21: CpuOps::Load16(HL, Memory::ReadWord(PC), 12); PC += 2; break; // LD HL,d16
		case 0x22: CpuOps::Write8(HL, A, 8); HL += 1; break; // LD (HL+),A
		case 0x23: CpuOps::Inc16(HL, 8); break; // INC HL
		case 0x24: CpuOps::Inc8(H, 4); break; // INC H
		case 0x25: CpuOps::Dec8(H, 4); break; // DEC H
		case 0x26: CpuOps::Load8(H, Memory::ReadByte(PC), 8); PC += 1; break; // LD H,d8
		case 0x27: CpuOps::Daa(4); break; // DAA
		case 0x28: CpuOps::JmpRel(Flags::Get(Flags::z), 8); break; // JR Z,r8
		case 0x29: CpuOps::Add16(HL, HL, 8); break; // ADD HL,HL
		case 0x2A: CpuOps::Load8(A, Memory::ReadByte(HL), 12); HL += 1; break; // LD A,(HL+)
		case 0x2B: CpuOps::Dec16(HL, 8); break; // DEC HL
		case 0x2C: CpuOps::Inc8(L, 4); break; // INC L
		case 0x2D: CpuOps::Dec8(L, 4); break; // DEC L
		case 0x2E: CpuOps::Load8(L, Memory::ReadByte(PC), 8); PC += 1; break; // LD L,d8
		case 0x2F: CpuOps::CmplA(4); break; // CPL A
		case 0x30: CpuOps::JmpRel(!Flags::Get(Flags::c), 8); break; // JR NC,r8
		case 0x31: CpuOps::Load16(SP, Memory::ReadWord(PC), 12); PC += 2; break; // LD SP,d16
		case 0x32: CpuOps::Write8(HL, A, 8); HL -= 1; break; // LD (HL-),A
		case 0x33: CpuOps::Inc16(SP, 8); break; // INC SP
		case 0x34: CpuOps::Inc8Mem(HL, 12); break; // INC (HL)
		case 0x35: CpuOps::Dec8Mem(HL, 12); break; // DEC (HL)
		case 0x36: CpuOps::Write8(HL, Memory::ReadByte(PC), 12); PC += 1; break; // LD (HL),d8
		case 0x37: CpuOps::Scf(4); break; // SCF
		case 0x38: CpuOps::JmpRel(Flags::Get(Flags::c), 8); break; // JR C,r8
		case 0x39: CpuOps::Add16(HL, SP, 8); break; // ADD HL,SP
		case 0x3A: CpuOps::Load8(A, Memory::ReadByte(HL), 8); HL -= 1; break; // LD A,(HL-)
		case 0x3B: CpuOps::Dec16(SP, 8); break; // DEC SP
		case 0x3C: CpuOps::Inc8(A, 4); break; // INC A
		case 0x3D: CpuOps::Dec8(A, 4); break; // DEC A
		case 0x3E: CpuOps::Load8(A, Memory::ReadByte(PC), 8); PC += 1; break; // LD A,d8
		case 0x3F: CpuOps::Ccf(4); break; // CCF
		case 0x40: CpuOps::Load8(B, B, 4); break; // LD B,B
		case 0x41: CpuOps::Load8(B, C, 4); break; // LD B,C
		case 0x42: CpuOps::Load8(B, D, 4); break; // LD B,D
		case 0x43: CpuOps::Load8(B, E, 4); break; // LD B,E
		case 0x44: CpuOps::Load8(B, H, 4); break; // LD B,H
		case 0x45: CpuOps::Load8(B, L, 4); break; // LD B,L
		case 0x46: CpuOps::Load8(B, Memory::ReadByte(HL), 8); break; // LD B,(HL)
		case 0x47: CpuOps::Load8(B, A, 4); break; // LD B,A
		case 0x48: CpuOps::Load8(C, B, 4); break; // LD C,B
		case 0x49: CpuOps::Load8(C, C, 4); break; // LD C,C
		case 0x4A: CpuOps::Load8(C, D, 4); break; // LD C,D
		case 0x4B: CpuOps::Load8(C, E, 4); break; // LD C,E
		case 0x4C: CpuOps::Load8(C, H, 4); break; // LD C,H
		case 0x4D: CpuOps::Load8(C, L, 4); break; // LD C,L
		case 0x4E: CpuOps::Load8(C, Memory::ReadByte(HL), 8); break; // LD C,(HL)
		case 0x4F: CpuOps::Load8(C, A, 4); break; // LD C,A
		case 0x50: CpuOps::Load8(D, B, 4); break; // LD D,B
		case 0x51: CpuOps::Load8(D, C, 4); break; // LD D,C
		case 0x52: CpuOps::Load8(D, D, 4); break; // LD D,D
		case 0x53: CpuOps::Load8(D, E, 4); break; // LD D,E
		case 0x54: CpuOps::Load8(D, H, 4); break; // LD D,H
		case 0x55: CpuOps::Load8(D, L, 4); break; // LD D,L
		case 0x56: CpuOps::Load8(D, Memory::ReadByte(HL), 8); break; // LD D,(HL)
		case 0x57: CpuOps::Load8(D, A, 4); break; // LD D,A
		case 0x58: CpuOps::Load8(E, B, 4); break; // LD E,B
		case 0x59: CpuOps::Load8(E, C, 4); break; // LD E,C
		case 0x5A: CpuOps::Load8(E, D, 4); break; // LD E,D
		case 0x5B: CpuOps::Load8(E, E, 4); break; // LD E,E
		case 0x5C: CpuOps::Load8(E, H, 4); break; // LD E,H
		case 0x5D: CpuOps::Load8(E, L, 4); break; // LD E,L
		case 0x5E: CpuOps::Load8(E, Memory::ReadByte(HL), 8); break; // LD E,(HL)
		case 0x5F: CpuOps::Load8(E, A, 4); break; // LD E,A
		case 0x60: CpuOps::Load8(H, B, 4); break; // LD H,B
		case 0x61: CpuOps::Load8(H, C, 4); break; // LD H,C
		case 0x62: CpuOps::Load8(H, D, 4); break; // LD H,D
		case 0x63: CpuOps::Load8(H, E, 4); break; // LD H,E
		case 0x64: CpuOps::Load8(H, H, 4); break; // LD H,H
		case 0x65: CpuOps::Load8(H, L, 4); break; // LD H,L
		case 0x66: CpuOps::Load8(H, Memory::ReadByte(HL), 8); break; // LD H,(HL)
		case 0x67: CpuOps::Load8(H, A, 4); break; // LD H,A
		case 0x68: CpuOps::Load8(L, B, 4); break; // LD L,B
		case 0x69: CpuOps::Load8(L, C, 4); break; // LD L,C
		case 0x6A: CpuOps::Load8(L, D, 4); break; // LD L,D
		case 0x6B: CpuOps::Load8(L, E, 4); break; // LD L,E
		case 0x6C: CpuOps::Load8(L, H, 4); break; // LD L,H
		case 0x6D: CpuOps::Load8(L, L, 4); break; // LD L,L
		case 0x6E: CpuOps::Load8(L, Memory::ReadByte(HL), 8); break; // LD L,(HL)
		case 0x6F: CpuOps::Load8(L, A, 4); break; // LD L,A
		case 0x70: CpuOps::Write8(HL, B, 8); break; // LD (HL),B
		case 0x71: CpuOps::Write8(HL, C, 8); break; // LD (HL),C
		case 0x72: CpuOps::Write8(HL, D, 8); break; // LD (HL),D
		case 0x73: CpuOps::Write8(HL, E, 8); break; // LD (HL),E
		case 0x74: CpuOps::Write8(HL, H, 8); break; // LD (HL),H
		case 0x75: CpuOps::Write8(HL, L, 8); break; // LD (HL),L
		case 0x76: CpuOps::Halt(4); break; // HALT
		case 0x77: CpuOps::Write8(HL, A, 8); break; // LD (HL),A
		case 0x78: CpuOps::Load8(A, B, 4); break; // LD A,B
		case 0x79: CpuOps::Load8(A, C, 4); break; // LD A,C
		case 0x7A: CpuOps::Load8(A, D, 4); break; // LD A,D
		case 0x7B: CpuOps::Load8(A, E, 4); break; // LD A,E
		case 0x7C: CpuOps::Load8(A, H, 4); break; // LD A,H
		case 0x7D: CpuOps::Load8(A, L, 4); break; // LD A,L
		case 0x7E: CpuOps::Load8(A, Memory::ReadByte(HL), 8); break; // LD A,(HL)
		case 0x7F: CpuOps::Load8(A, A, 4); break; // LD A,A
		case 0x80: CpuOps::Add8(A, B, 4); break; // ADD A,B
		case 0x81: CpuOps::Add8(A, C, 4); break; // ADD A,C
		case 0x82: CpuOps::Add8(A, D, 4); break; // ADD A,D
		case 0x83: CpuOps::Add8(A, E, 4); break; // ADD A,E
		case 0x84: CpuOps::Add8(A, H, 4); break; // ADD A,H
		case 0x85: CpuOps::Add8(A, L, 4); break; // ADD A,L
		case 0x86: CpuOps::Add8(A, Memory::ReadByte(HL), 8); break; // ADD A,(HL)
		case 0x87: CpuOps::Add8(A, A, 4); break; // ADD A,A
		case 0x88: CpuOps::Adc8(A, B, 4); break; // ADC A,B
		case 0x89: CpuOps::Adc8(A, C, 4); break; // ADC A,C
		case 0x8A: CpuOps::Adc8(A, D, 4); break; // ADC A,D
		case 0x8B: CpuOps::Adc8(A, E, 4); break; // ADC A,E
		case 0x8C: CpuOps::Adc8(A, H, 4); break; // ADC A,H
		case 0x8D: CpuOps::Adc8(A, L, 4); break; // ADC A,L
		case 0x8E: CpuOps::Adc8(A, Memory::ReadByte(HL), 8); break; // ADC A,(HL)
		case 0x8F: CpuOps::Adc8(A, A, 4); break; // ADC A,A
		case 0x90: CpuOps::Sub8(A, B, 4); break; // SUB A,B
		case 0x91: CpuOps::Sub8(A, C, 4); break; // SUB A,C
		case 0x92: CpuOps::Sub8(A, D, 4); break; // SUB A,D
		case 0x93: CpuOps::Sub8(A, E, 4); break; // SUB A,E
		case 0x94: CpuOps::Sub8(A, H, 4); break; // SUB A,H
		case 0x95: CpuOps::Sub8(A, L, 4); break; // SUB A,L
		case 0x96: CpuOps::Sub8(A, Memory::ReadByte(HL), 8); break; // SUB A,(HL)
		case 0x97: CpuOps::Sub8(A, A, 4); break; // SUB A,A
		case 0x98: CpuOps::Sbc8(A, B, 4); break; // SBC A,B
		case 0x99: CpuOps::Sbc8(A, C, 4); break; // SBC A,C
		case 0x9A: CpuOps::Sbc8(A, D, 4); break; // SBC A,D
		case 0x9B: CpuOps::Sbc8(A, E, 4); break; // SBC A,E
		case 0x9C: CpuOps::Sbc8(A, H, 4); break; // SBC A,H
		case 0x9D: CpuOps::Sbc8(A, L, 4); break; // SBC A,L
		case 0x9E: CpuOps::Sbc8(A, Memory::ReadByte(HL), 8); break; // SBC A,(HL)
		case 0x9F: CpuOps::Sbc8(A, A, 4); break; // SBC A,A
		case 0xA0: CpuOps::And8(A, B, 4); break; // AND A,B
		case 0xA1: CpuOps::And8(A, C, 4); break; // AND A,C
		case 0xA2: CpuOps::And8(A, D, 4); break; // AND A,D
		case 0xA3: CpuOps::And8(A, E, 4); break; // AND A,E
		case 0xA4: CpuOps::And8(A, H, 4); break; // AND A,H
		case 0xA5: CpuOps::And8(A, L, 4); break; // AND A,L
		case 0xA6: CpuOps::And8(A, Memory::ReadByte(HL), 8); break; // AND A,(HL)
		case 0xA7: CpuOps::And8(A, A, 4); break; // AND A,A
		case 0xA8: CpuOps::Xor8(A, B, 4); break; // XOR A,B
		case 0xA9: CpuOps::Xor8(A, C, 4); break; // XOR A,C
		case 0xAA: CpuOps::Xor8(A, D, 4); break; // XOR A,D
		case 0xAB: CpuOps::Xor8(A, E, 4); break; // XOR A,E
		case 0xAC: CpuOps::Xor8(A, H, 4); break; // XOR A,H
		case 0xAD: CpuOps::Xor8(A, L, 4); break; // XOR A,L
		case 0xAE: CpuOps::Xor8(A, Memory::ReadByte(HL), 8); break; // XOR A,(HL)
		case 0xAF: CpuOps::Xor8(A, A, 4); break; // XOR A,A
		case 0xB0: CpuOps::Or8(A, B, 4); break; // OR A,B
		case 0xB1: CpuOps::Or8(A, C, 4); break; // OR A,C
		case 0xB2: CpuOps::Or8(A, D, 4); break; // OR A,D
		case 0xB3: CpuOps::Or8(A, E, 4); break; // OR A,E
		case 0xB4: CpuOps::Or8(A, H, 4); break; // OR A,H
		case 0xB5: CpuOps::Or8(A, L, 4); break; // OR A,L
		case 0xB6: CpuOps::Or8(A, Memory::ReadByte(HL), 8); break; // OR A,(HL)
		case 0xB7: CpuOps::Or8(A, A, 4); break; // OR A,A
		case 0xB8: CpuOps::Cmp8(A, B, 4); break; // CP A,B
		case 0xB9: CpuOps::Cmp8(A, C, 4); break; // CP A,C
		case 0xBA: CpuOps::Cmp8(A, D, 4); break; // CP A,D
		case 0xBB: CpuOps::Cmp8(A, E, 4); break; // CP A,E
		case 0xBC: CpuOps::Cmp8(A, H, 4); break; // CP A,H
		case 0xBD: CpuOps::Cmp8(A, L, 4); break; // CP A,L
		case 0xBE: CpuOps::Cmp8(A, Memory::ReadByte(HL), 8); break; // CP A,(HL)
		case 0xBF: CpuOps::Cmp8(A, A, 4); break; // CP A,A
		case 0xC0: CpuOps::Ret(!Flags::Get(Flags::z), 8); break; // RET NZ
		case 0xC1: BC = Memory::Pop(); cycles += 12; break; // POP BC
		case 0xC2: CpuOps::JmpImm(!Flags::Get(Flags::z), 12); break; // JP NZ,a16
		case 0xC3: CpuOps::JmpImm(true, 12); break; // JP a16
		case 0xC4: CpuOps::Call(!Flags::Get(Flags::z), 12); break; // CALL NZ,a16
		case 0xC5: Memory::Push(bc); cycles += 16; break; // PUSH BC
		case 0xC6: CpuOps::Add8(A, Memory::ReadByte(PC), 8); PC += 1; break; // ADD A,d8
		case 0xC7: CpuOps::Rst(0x00, 16); break; // RST 00H
		case 0xC8: CpuOps::Ret(Flags::Get(Flags::z), 8); break; // RET Z
		case 0xC9: CpuOps::Ret(true, 8); break; // RET
		case 0xCA: CpuOps::JmpImm(Flags::Get(Flags::z), 12); break; // JP Z,a16
		case 0xCB: ExecuteExtendedOpcode(); cycles += 4; break; // PREFIX CB
		case 0xCC: CpuOps::Call(Flags::Get(Flags::z), 12); break; // CALL Z,a16
		case 0xCD: CpuOps::Call(true, 12); break; // CALL a16
		case 0xCE: CpuOps::Adc8(A, Memory::ReadByte(PC), 8); PC += 1; break; // ADC A,d8
		case 0xCF: CpuOps::Rst(0x08, 16); break; // RST 08H
		case 0xD0: CpuOps::Ret(!Flags::Get(Flags::c), 8); break; // RET NC
		case 0xD1: DE = Memory::Pop(); cycles += 12; break; // POP DE
		case 0xD2: CpuOps::JmpImm(!Flags::Get(Flags::c), 12); break; // JP NC,a16
		case 0xD4: CpuOps::Call(!Flags::Get(Flags::c), 12); break; // CALL NC,a16
		case 0xD5: Memory::Push(de); cycles += 16; break; // PUSH DE
		case 0xD6: CpuOps::Sub8(A, Memory::ReadByte(PC), 8); PC += 1; break; // SUB A, d8
		case 0xD7: CpuOps::Rst(0x10, 16); break; // RST 10H
		case 0xD8: CpuOps::Ret(Flags::Get(Flags::c), 8); break; // RET C
		case 0xD9: CpuOps::Ret(true, 8); break; // RETI
		case 0xDA: CpuOps::JmpImm(Flags::Get(Flags::c), 12); break; // JP C,a16
		case 0xDC: CpuOps::Call(Flags::Get(Flags::c), 12); break; // CALL C,a16
		case 0xDE: CpuOps::Sbc8(A, Memory::ReadByte(PC), 8); PC += 1; break; // SBC A,d8
		case 0xDF: CpuOps::Rst(0x18, 16); break; // RST 18H
		case 0xE0: CpuOps::Write8(0xFF00 + Memory::ReadByte(PC), A, 12); PC += 1; break; // LDH (a8),A
		case 0xE1: HL = Memory::Pop(); cycles += 12; break; // POP HL
		case 0xE2: CpuOps::Write8(0xFF00 + Memory::ReadByte(C), A, 8); break; // LD (C),A
		case 0xE5: Memory::Push(hl); cycles += 16; break; // PUSH HL
		case 0xE6: CpuOps::And8(A, Memory::ReadByte(PC), 8); PC += 1; break; // AND A, d8
		case 0xE7: CpuOps::Rst(0x20, 16); break; // RST 20H
		case 0xE8: CpuOps::AddSpR8(Memory::ReadByte(PC), 16); PC += 1; break; // ADD SP,r8
		case 0xE9: PC = HL; cycles += 4; break; // JP (HL)
		case 0xEA: CpuOps::Write8(Memory::ReadWord(PC), A, 16); PC += 2; break; // LD (a16),A
		case 0xEE: CpuOps::Xor8(A, Memory::ReadByte(PC), 8); PC += 1; break; // XOR A, d8
		case 0xEF: CpuOps::Rst(0x28, 16); break; // RST 28H
		case 0xF0: CpuOps::Load8(A, Memory::ReadByte(0xFF00 + Memory::ReadByte(PC)), 12); PC += 1; break; // LDH A,(a8)
		case 0xF1: AF = (Memory::Pop() & ~0xF); cycles += 12; break; // POP AF
		case 0xF2: CpuOps::Load8(A, Memory::ReadByte(0xFF00 + C), 8); break; // LD A,(C)
		case 0xF3: CpuOps::DI(4); break; // DI
		case 0xF5: Memory::Push(af); cycles += 16; break; // PUSH AF
		case 0xF6: CpuOps::Or8(A, Memory::ReadByte(PC), 8); PC += 1; break; // OR A, d8
		case 0xF7: CpuOps::Rst(0x30, 16); break; // RST 30H
		case 0xF8: CpuOps::LoadHlSpR8(Memory::ReadByte(PC), 12); PC += 1; break;// LD HL,SP+r8
		case 0xF9: CpuOps::Load16(SP, HL, 8); break; // LD SP,HL
		case 0xFA: CpuOps::Load8(A, Memory::ReadByte(Memory::ReadWord(PC)), 16); PC += 2; break; // LD A,(a16)
		case 0xFB: CpuOps::EI(4); break; // EI
		case 0xFE: CpuOps::Cmp8(A, Memory::ReadByte(PC), 8); PC += 1; break; // CP A, d8
		case 0xFF: CpuOps::Rst(0x38, 16); break; // RST 38H
		default: Log::Critical("unimplemented opcode %02X", opcode); break;
	}
}

// responsible for executing extended opcodes (prefix CB)
void Cpu::ExecuteExtendedOpcode()
{
	u8 opcode = Memory::ReadByte(PC);
	instructionsRan += 1;
	PC += 1;

	switch(opcode)
	{
		case 0x00: CpuOps::Rlc8(B, true, 8); break; // RLC B
		case 0x01: CpuOps::Rlc8(C, true, 8); break; // RLC C
		case 0x02: CpuOps::Rlc8(D, true, 8); break; // RLC D
		case 0x03: CpuOps::Rlc8(E, true, 8); break; // RLC E
		case 0x04: CpuOps::Rlc8(H, true, 8); break; // RLC H
		case 0x05: CpuOps::Rlc8(L, true, 8); break; // RLC L
		case 0x06: CpuOps::Rlc8Mem(HL, true, 16); break; // RLC (HL)
		case 0x07: CpuOps::Rlc8(A, true, 8); break; // RLC A
		case 0x08: CpuOps::Rrc8(B, true, 8); break; // RRC B
		case 0x09: CpuOps::Rrc8(C, true, 8); break; // RRC C
		case 0x0A: CpuOps::Rrc8(D, true, 8); break; // RRC D
		case 0x0B: CpuOps::Rrc8(E, true, 8); break; // RRC E
		case 0x0C: CpuOps::Rrc8(H, true, 8); break; // RRC H
		case 0x0D: CpuOps::Rrc8(L, true, 8); break; // RRC L
		case 0x0E: CpuOps::Rrc8Mem(HL, true, 16); break; // RRC (HL)
		case 0x0F: CpuOps::Rrc8(A, true, 8); break; // RRC A
		case 0x10: CpuOps::Rl8(B, true, 8); break; // RL B
		case 0x11: CpuOps::Rl8(C, true, 8); break; // RL C
		case 0x12: CpuOps::Rl8(D, true, 8); break; // RL D
		case 0x13: CpuOps::Rl8(E, true, 8); break; // RL E
		case 0x14: CpuOps::Rl8(H, true, 8); break; // RL H
		case 0x15: CpuOps::Rl8(L, true, 8); break; // RL L
		case 0x16: CpuOps::Rl8Mem(HL, true, 16); break; // RL (HL)
		case 0x17: CpuOps::Rl8(A, true, 8); break; // RL A
		case 0x18: CpuOps::Rr8(B, true, 8); break; // RR B
		case 0x19: CpuOps::Rr8(C, true, 8); break; // RR C
		case 0x1A: CpuOps::Rr8(D, true, 8); break; // RR D
		case 0x1B: CpuOps::Rr8(E, true, 8); break; // RR E
		case 0x1C: CpuOps::Rr8(H, true, 8); break; // RR H
		case 0x1D: CpuOps::Rr8(L, true, 8); break; // RR L
		case 0x1E: CpuOps::Rr8Mem(HL, true, 16); break; // RR (HL)
		case 0x1F: CpuOps::Rr8(A, true, 8); break; // RR A
		case 0x20: CpuOps::Slc8(B, 8); break; // SLA B
		case 0x21: CpuOps::Slc8(C, 8); break; // SLA C
		case 0x22: CpuOps::Slc8(D, 8); break; // SLA D
		case 0x23: CpuOps::Slc8(E, 8); break; // SLA E
		case 0x24: CpuOps::Slc8(H, 8); break; // SLA H
		case 0x25: CpuOps::Slc8(L, 8); break; // SLA L
		case 0x26: CpuOps::Slc8Mem(HL, 16); break; // SLA (HL)
		case 0x27: CpuOps::Slc8(A, 8); break; // SLA A
		case 0x28: CpuOps::Src8(B, 8); break; // SRA B
		case 0x29: CpuOps::Src8(C, 8); break; // SRA C
		case 0x2A: CpuOps::Src8(D, 8); break; // SRA D
		case 0x2B: CpuOps::Src8(E, 8); break; // SRA E
		case 0x2C: CpuOps::Src8(H, 8); break; // SRA H
		case 0x2D: CpuOps::Src8(L, 8); break; // SRA L
		case 0x2E: CpuOps::Src8Mem(HL, 16); break; // SRA (HL)
		case 0x2F: CpuOps::Src8(A, 8); break; // SRA A
		case 0x30: CpuOps::BitSwap(B, 8); break; // SWAP B
		case 0x31: CpuOps::BitSwap(C, 8); break; // SWAP C
		case 0x32: CpuOps::BitSwap(D, 8); break; // SWAP D
		case 0x33: CpuOps::BitSwap(E, 8); break; // SWAP E
		case 0x34: CpuOps::BitSwap(H, 8); break; // SWAP H
		case 0x35: CpuOps::BitSwap(L, 8); break; // SWAP L
		case 0x36: CpuOps::BitSwapMem(HL, 16); break; // SWAP (HL)
		case 0x37: CpuOps::BitSwap(A, 8); break; // SWAP A
		case 0x38: CpuOps::Rrc8(B, true, 8); break; // SRL B
		case 0x39: CpuOps::Rrc8(C, true, 8); break; // SRL C
		case 0x3A: CpuOps::Rrc8(D, true, 8); break; // SRL D
		case 0x3B: CpuOps::Rrc8(E, true, 8); break; // SRL E
		case 0x3C: CpuOps::Rrc8(H, true, 8); break; // SRL H
		case 0x3D: CpuOps::Rrc8(L, true, 8); break; // SRL L
		case 0x3E: CpuOps::Rrc8Mem(HL, true, 16); break; // SRL (HL)
		case 0x3F: CpuOps::Rrc8(A, true, 8); break; // SRL A

		case 0x40 ... 0x7F:
		{
			u8 bit = (((opcode >> 4) - 4) + 1);
			if (bit == 1) bit = 0;

			switch(opcode & 0xF)
			{
				case 0x0: CpuOps::BitTest(B, bit, 8); break; // BIT B,x
				case 0x1: CpuOps::BitTest(C, bit , 8); break; // BIT C,x
				case 0x2: CpuOps::BitTest(D, bit, 8); break; // BIT D,x
				case 0x3: CpuOps::BitTest(E, bit, 8); break; // BIT E,x
				case 0x4: CpuOps::BitTest(H, bit, 8); break; // BIT H,x
				case 0x5: CpuOps::BitTest(L, bit, 8); break;  // BIT L,x
				case 0x6: CpuOps::BitTestMem(HL, bit, 16); break; // BIT HL,x
				case 0x7: CpuOps::BitTest(A, bit, 8); break; // BIT A,x
				case 0x8: CpuOps::BitTest(B, bit + 1, 8); break; // BIT B,x
				case 0x9: CpuOps::BitTest(C, bit + 1, 8); break; // BIT C,x
				case 0xA: CpuOps::BitTest(D, bit + 1, 8); break; // BIT D,x
				case 0xB: CpuOps::BitTest(E, bit + 1, 8); break; // BIT E,x
				case 0xC: CpuOps::BitTest(H, bit + 1, 8); break; // BIT H,x
				case 0xD: CpuOps::BitTest(L, bit + 1, 8); break; // BIT L,x
				case 0xE: CpuOps::BitTestMem(HL, bit + 1, 16); break; // BIT HL,x
				case 0xF: CpuOps::BitTest(A, bit + 1, 8); break; // BIT A,x
			}
		}
		break;

		case 0x80 ... 0xBF:
		{
			u8 bit = (((opcode >> 4) - 8) + 1);
			if (bit == 1) bit = 0;

			switch(opcode & 0xF)
			{
				case 0x0: CpuOps::BitClear(B, bit, 8); break; // RES B,x
				case 0x1: CpuOps::BitClear(C, bit, 8); break; // RES C,x
				case 0x2: CpuOps::BitClear(D, bit, 8); break; // RES D,x
				case 0x3: CpuOps::BitClear(E, bit, 8); break; // RES E,x
				case 0x4: CpuOps::BitClear(H, bit, 8); break; // RES H,x
				case 0x5: CpuOps::BitClear(L, bit, 8); break;  // RES L,x
				case 0x6: CpuOps::BitClearMem(HL, bit, 16); break; // RES HL,x
				case 0x7: CpuOps::BitClear(A, bit, 8); break; // RES A,x
				case 0x8: CpuOps::BitClear(B, bit + 1, 8); break; // RES B,x
				case 0x9: CpuOps::BitClear(C, bit + 1, 8); break; // RES C,x
				case 0xA: CpuOps::BitClear(D, bit + 1, 8); break; // RES D,x
				case 0xB: CpuOps::BitClear(E, bit + 1, 8); break; // RES E,x
				case 0xC: CpuOps::BitClear(H, bit + 1, 8); break; // RES H,x
				case 0xD: CpuOps::BitClear(L, bit + 1, 8); break; // RES L,x
				case 0xE: CpuOps::BitClearMem(HL, bit + 1, 16); break; // RES HL,x
				case 0xF: CpuOps::BitClear(A, bit + 1, 8); break; // RES A,x
			}
		}
		break;

		case 0xC0 ... 0xFF:
		{
			u8 bit = (((opcode >> 4) - 12) + 1);
			if (bit == 1) bit = 0;

			switch(opcode & 0xF)
			{
				case 0x0: CpuOps::BitSet(B, bit, 8); break; // SET B,x
				case 0x1: CpuOps::BitSet(C, bit, 8); break; // SET C,x
				case 0x2: CpuOps::BitSet(D, bit, 8); break; // SET D,x
				case 0x3: CpuOps::BitSet(E, bit, 8); break; // SET E,x
				case 0x4: CpuOps::BitSet(H, bit, 8); break; // SET H,x
				case 0x5: CpuOps::BitSet(L, bit, 8); break;  // SET L,x
				case 0x6: CpuOps::BitSetMem(HL, bit, 16); break; // SET HL,x
				case 0x7: CpuOps::BitSet(A, bit, 8); break; // SET A,x
				case 0x8: CpuOps::BitSet(B, bit + 1, 8); break; // SET B,x
				case 0x9: CpuOps::BitSet(C, bit + 1, 8); break; // SET C,x
				case 0xA: CpuOps::BitSet(D, bit + 1, 8); break; // SET D,x
				case 0xB: CpuOps::BitSet(E, bit + 1, 8); break; // SET E,x
				case 0xC: CpuOps::BitSet(H, bit + 1, 8); break; // SET H,x
				case 0xD: CpuOps::BitSet(L, bit + 1, 8); break; // SET L,x
				case 0xE: CpuOps::BitSetMem(HL, bit + 1, 16); break; // SET HL,x
				case 0xF: CpuOps::BitSet(A, bit + 1, 8); break; // SET A,x
			}
		}
		break;

		default: Log::Critical("unimplemented (prefix-CB) opcode %02X", opcode);
	}
}

// responsible for loading save states
bool Cpu::LoadState(unsigned int num)
{
	char val[512];
	char memFilename[512];
	char regFilename[512];
	int index = 0;

	sprintf(memFilename, "state_mem_%d.bin", num);
	sprintf(regFilename, "state_reg_%d.bin", num);

	FILE *fp = fopen(memFilename, "rb");
	FILE *fp2 = fopen(regFilename, "r");

	if (fp == NULL || fp2 == NULL) return false;

	fread(&Memory::mem, 1, 0x10000, fp);

	while(fscanf(fp2, "%s\n", val) != EOF)
	{
		switch(index)
		{
			case 0: AF = (u16)strtol(val, NULL, 16); break;
			case 1: BC = (u16)strtol(val, NULL, 16); break;
			case 2: DE = (u16)strtol(val, NULL, 16); break;
			case 3: HL = (u16)strtol(val, NULL, 16); break;
			case 4: PC = (u16)strtol(val, NULL, 16); break;
			case 5: SP = (u16)strtol(val, NULL, 16); break;
			case 6: cycles = (int)strtol(val, NULL, 10); break;
			case 7: pendingInterrupt = (int)strtol(val, NULL, 10); break;
			case 8: halted = (int)strtol(val, NULL, 10); break;
			case 9: stopped = (int)strtol(val, NULL, 10); break;
			case 10: instructionsRan = (int)strtol(val, NULL, 10); break;
		}

		index++;
	}

	fclose(fp);
	fclose(fp2);

	return true;
}

// responsible for saving save states
void Cpu::SaveState(unsigned int num)
{
	char memFilename[512];
	char regFilename[512];

	sprintf(memFilename, "state_mem_%d.bin", num);
	sprintf(regFilename, "state_reg_%d.bin", num);

	FILE *fp = fopen(memFilename, "wb");
	FILE *fp2 = fopen(regFilename, "w");

	fwrite(Memory::mem, sizeof(Memory::mem), 1, fp);

	// save registers
	fprintf(fp2, "%04X\n", AF);
	fprintf(fp2, "%04X\n", BC);
	fprintf(fp2, "%04X\n", DE);
	fprintf(fp2, "%04X\n", HL);
	fprintf(fp2, "%04X\n", PC);
	fprintf(fp2, "%04X\n", SP);
	// save misc
	fprintf(fp2, "%d\n", cycles);
	fprintf(fp2, "%d\n", pendingInterrupt);
	fprintf(fp2, "%d\n", halted);
	fprintf(fp2, "%d\n", stopped);
	fprintf(fp2, "%d\n", instructionsRan);

	fclose(fp);
	fclose(fp2);
}
