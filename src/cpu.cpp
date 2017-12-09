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
		case 0x07: CpuOps::Rlc8(A, 4); break; // RLCA
		case 0x08: Memory::WriteWord(Memory::ReadWord(PC), sp); PC += 2; cycles += 20; break; // LD (a16),SP
		case 0x09: CpuOps::Add16(HL, BC, 8); break; // ADD HL,BC
		case 0x0A: CpuOps::Load8(A, Memory::ReadByte(BC), 8); break; // LD A,(BC)
		case 0x0B: CpuOps::Dec16(BC, 8); break; // DEC BC
		case 0x0C: CpuOps::Inc8(C, 4); break; // INC C
		case 0x0D: CpuOps::Dec8(C, 4); break; // DEC C
		case 0x0E: CpuOps::Load8(C, Memory::ReadByte(PC), 8); PC += 1; break; // LD C,d8
		case 0x0F: CpuOps::Rrc8(A, 4); break; // RRCA
		case 0x10: CpuOps::Stop(4); break; // STOP
		case 0x11: CpuOps::Load16(DE, Memory::ReadWord(PC), 12); PC += 2; break; // LD DE,d16
		case 0x12: CpuOps::Write8(DE, A, 8); break; // LD (DE),A
		case 0x13: CpuOps::Inc16(DE, 8); break; // INC DE
		case 0x14: CpuOps::Inc8(D, 4); break; // INC D
		case 0x15: CpuOps::Dec8(D, 4); break; // DEC D
		case 0x16: CpuOps::Load8(D, Memory::ReadByte(PC), 8); PC += 1; break; // LD D,d8
		case 0x17: CpuOps::Rl8(A, 4); break; // RLA
		case 0x18: CpuOps::JmpRel(true, 8); break; // JR r8
		case 0x19: CpuOps::Add16(HL, DE, 8); break; // ADD HL,DE
		case 0x1A: CpuOps::Load8(A, Memory::ReadByte(DE), 8); break; // LD A,(DE)
		case 0x1B: CpuOps::Dec16(DE, 8); break; // DEC DE
		case 0x1C: CpuOps::Inc8(E, 4); break; // INC E
		case 0x1D: CpuOps::Dec8(E, 4); break; // DEC E
		case 0x1E: CpuOps::Load8(E, Memory::ReadByte(PC), 8); PC += 1; break; // LD E,d8
		case 0x1F: CpuOps::Rr8(A, 4); break; // RRA
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
		case 0x34: CpuOps::Inc8Mem(HL, 8); break; // INC (HL)
		case 0x35: CpuOps::Dec8Mem(HL, 8); break; // DEC (HL)
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
		case 0xFA: CpuOps::Load8(A, Memory::ReadByte(Memory::ReadWord(PC)), 12); PC += 2; break; // LD A,(a16)
		case 0xFB: CpuOps::EI(4); break; // EI
		case 0xFE: CpuOps::Cmp8(A, Memory::ReadByte(PC), 8); PC += 1; break; // CP A, d8
		case 0xFF: CpuOps::Rst(0x38, 16); break; // RST 38H
	}
}

// responsible for executing extended opcodes (prefix CB)
void Cpu::ExecuteExtendedOpcode()
{
	u8 opcode = Memory::ReadByte(PC);
	instructionsRan += 1;
	PC += 1;

	printf("unimplemented CB opcode: %02X\n", opcode);

	switch(opcode)
	{
		case 0x00: break; //
		case 0x01: break; //
		case 0x02: break; //
		case 0x03: break; //
		case 0x04: break; //
		case 0x05: break; //
		case 0x06: break; //
		case 0x07: break; //
		case 0x08: break; //
		case 0x09: break; //
		case 0x0A: break; //
		case 0x0B: break; //
		case 0x0C: break; //
		case 0x0D: break; //
		case 0x0E: break; //
		case 0x0F: break; //
		case 0x10: break; //
		case 0x11: break; //
		case 0x12: break; //
		case 0x13: break; //
		case 0x14: break; //
		case 0x15: break; //
		case 0x16: break; //
		case 0x17: break; //
		case 0x18: break; //
		case 0x19: break; //
		case 0x1A: break; //
		case 0x1B: break; //
		case 0x1C: break; //
		case 0x1D: break; //
		case 0x1E: break; //
		case 0x1F: break; //
		case 0x20: break; //
		case 0x21: break; //
		case 0x22: break; //
		case 0x23: break; //
		case 0x24: break; //
		case 0x25: break; //
		case 0x26: break; //
		case 0x27: break; //
		case 0x28: break; //
		case 0x29: break; //
		case 0x2A: break; //
		case 0x2B: break; //
		case 0x2C: break; //
		case 0x2D: break; //
		case 0x2E: break; //
		case 0x2F: break; //
		case 0x30: break; //
		case 0x31: break; //
		case 0x32: break; //
		case 0x33: break; //
		case 0x34: break; //
		case 0x35: break; //
		case 0x36: break; //
		case 0x37: break; //
		case 0x38: break; //
		case 0x39: break; //
		case 0x3A: break; //
		case 0x3B: break; //
		case 0x3C: break; //
		case 0x3D: break; //
		case 0x3E: break; //
		case 0x3F: break; //
		case 0x40: break; //
		case 0x41: break; //
		case 0x42: break; //
		case 0x43: break; //
		case 0x44: break; //
		case 0x45: break; //
		case 0x46: break; //
		case 0x47: break; //
		case 0x48: break; //
		case 0x49: break; //
		case 0x4A: break; //
		case 0x4B: break; //
		case 0x4C: break; //
		case 0x4D: break; //
		case 0x4E: break; //
		case 0x4F: break; //
		case 0x50: break; //
		case 0x51: break; //
		case 0x52: break; //
		case 0x53: break; //
		case 0x54: break; //
		case 0x55: break; //
		case 0x56: break; //
		case 0x57: break; //
		case 0x58: break; //
		case 0x59: break; //
		case 0x5A: break; //
		case 0x5B: break; //
		case 0x5C: break; //
		case 0x5D: break; //
		case 0x5E: break; //
		case 0x5F: break; //
		case 0x60: break; //
		case 0x61: break; //
		case 0x62: break; //
		case 0x63: break; //
		case 0x64: break; //
		case 0x65: break; //
		case 0x66: break; //
		case 0x67: break; //
		case 0x68: break; //
		case 0x69: break; //
		case 0x6A: break; //
		case 0x6B: break; //
		case 0x6C: break; //
		case 0x6D: break; //
		case 0x6E: break; //
		case 0x6F: break; //
		case 0x70: break; //
		case 0x71: break; //
		case 0x72: break; //
		case 0x73: break; //
		case 0x74: break; //
		case 0x75: break; //
		case 0x76: break; //
		case 0x77: break; //
		case 0x78: break; //
		case 0x79: break; //
		case 0x7A: break; //
		case 0x7B: break; //
		case 0x7C: break; //
		case 0x7D: break; //
		case 0x7E: break; //
		case 0x7F: break; //
		case 0x80: break; //
		case 0x81: break; //
		case 0x82: break; //
		case 0x83: break; //
		case 0x84: break; //
		case 0x85: break; //
		case 0x86: break; //
		case 0x87: break; //
		case 0x88: break; //
		case 0x89: break; //
		case 0x8A: break; //
		case 0x8B: break; //
		case 0x8C: break; //
		case 0x8D: break; //
		case 0x8E: break; //
		case 0x8F: break; //
		case 0x90: break; //
		case 0x91: break; //
		case 0x92: break; //
		case 0x93: break; //
		case 0x94: break; //
		case 0x95: break; //
		case 0x96: break; //
		case 0x97: break; //
		case 0x98: break; //
		case 0x99: break; //
		case 0x9A: break; //
		case 0x9B: break; //
		case 0x9C: break; //
		case 0x9D: break; //
		case 0x9E: break; //
		case 0x9F: break; //
		case 0xA0: break; //
		case 0xA1: break; //
		case 0xA2: break; //
		case 0xA3: break; //
		case 0xA4: break; //
		case 0xA5: break; //
		case 0xA6: break; //
		case 0xA7: break; //
		case 0xA8: break; //
		case 0xA9: break; //
		case 0xAA: break; //
		case 0xAB: break; //
		case 0xAC: break; //
		case 0xAD: break; //
		case 0xAE: break; //
		case 0xAF: break; //
		case 0xB0: break; //
		case 0xB1: break; //
		case 0xB2: break; //
		case 0xB3: break; //
		case 0xB4: break; //
		case 0xB5: break; //
		case 0xB6: break; //
		case 0xB7: break; //
		case 0xB8: break; //
		case 0xB9: break; //
		case 0xBA: break; //
		case 0xBB: break; //
		case 0xBC: break; //
		case 0xBD: break; //
		case 0xBE: break; //
		case 0xBF: break; //
		case 0xC0: break; //
		case 0xC1: break; //
		case 0xC2: break; //
		case 0xC3: break; //
		case 0xC4: break; //
		case 0xC5: break; //
		case 0xC6: break; //
		case 0xC7: break; //
		case 0xC8: break; //
		case 0xC9: break; //
		case 0xCA: break; //
		case 0xCB: break; //
		case 0xCC: break; //
		case 0xCD: break; //
		case 0xCE: break; //
		case 0xCF: break; //
		case 0xD0: break; //
		case 0xD1: break; //
		case 0xD2: break; //
		case 0xD3: break; //
		case 0xD4: break; //
		case 0xD5: break; //
		case 0xD6: break; //
		case 0xD7: break; //
		case 0xD8: break; //
		case 0xD9: break; //
		case 0xDA: break; //
		case 0xDB: break; //
		case 0xDC: break; //
		case 0xDD: break; //
		case 0xDE: break; //
		case 0xDF: break; //
		case 0xE0: break; //
		case 0xE1: break; //
		case 0xE2: break; //
		case 0xE3: break; //
		case 0xE4: break; //
		case 0xE5: break; //
		case 0xE6: break; //
		case 0xE7: break; //
		case 0xE8: break; //
		case 0xE9: break; //
		case 0xEA: break; //
		case 0xEB: break; //
		case 0xEC: break; //
		case 0xED: break; //
		case 0xEE: break; //
		case 0xEF: break; //
		case 0xF0: break; //
		case 0xF1: break; //
		case 0xF2: break; //
		case 0xF3: break; //
		case 0xF4: break; //
		case 0xF5: break; //
		case 0xF6: break; //
		case 0xF7: break; //
		case 0xF8: break; //
		case 0xF9: break; //
		case 0xFA: break; //
		case 0xFB: break; //
		case 0xFC: break; //
		case 0xFD: break; //
		case 0xFE: break; //
		case 0xFF: break; //
	}
}

// responsible for loading save states
void Cpu::LoadState()
{
	FILE *fp = fopen("state1.bin", "r");
	char val[512];
	int i = 0;

	while(fscanf(fp, "%s\n", val) != EOF)
	{
		if (i == 0)
			AF = (u16)strtol(val, NULL, 16);
		else if (i == 1)
			BC = (u16)strtol(val, NULL, 16);
		else if (i == 2)
			DE = (u16)strtol(val, NULL, 16);
		else if (i == 3)
			HL = (u16)strtol(val, NULL, 16);
		else if (i == 4)
			PC = (u16)strtol(val, NULL, 16);
		else if (i == 5)
			SP = (u16)strtol(val, NULL, 16);
		else if (i == 6)
			cycles = (int)strtol(val, NULL, 16);
		else if (i == 7)
			pendingInterrupt = (int)strtol(val, NULL, 16);
		else if (i == 8)
			halted = (int)strtol(val, NULL, 16);
		else if (i == 9)
			stopped = (int)strtol(val, NULL, 16);
		else if (i >= 10)
			Memory::WriteByte(0x0000 + (i - 10), (u8)strtol(val, NULL, 16));

		i++;
	}

	fclose(fp);
}

// responsible for saving save states
void Cpu::SaveState()
{
	FILE *fp = fopen("state1.bin", "w");
	// save registers
	fprintf(fp, "%04X\n", AF);
	fprintf(fp, "%04X\n", BC);
	fprintf(fp, "%04X\n", DE);
	fprintf(fp, "%04X\n", HL);
	fprintf(fp, "%04X\n", PC);
	fprintf(fp, "%04X\n", SP);
	// save misc
	fprintf(fp, "%d\n", cycles);
	fprintf(fp, "%d\n", pendingInterrupt);
	fprintf(fp, "%d\n", halted);
	fprintf(fp, "%d\n", stopped);
	// save the memory
	for (int i = 0x0000; i < 0x10000; i++)
	{
		fprintf(fp, "%02X\n", Memory::ReadByte(i));
	}

	fclose(fp);
}
