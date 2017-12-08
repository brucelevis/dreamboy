/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef CPU_H
#define CPU_H

// includes
#include "typedefs.h"

// definitions
#define MAX_CYCLES 4194304

class Cpu
{
	public:
		static void Init(bool useBios);
		static void ExecuteOpcode();
		static void LoadState();
		static void SaveState();

	private:
		static void ExecuteExtendedOpcode();

	public:
		union Register
		{
			u16 reg;
			struct
			{
				u8 hi, lo;
			};
		};

		static Register af;
		static Register bc;
		static Register de;
		static Register hl;
		static Register sp;
		static Register pc;
		static u8 opcode;
		static int cycles;
		static bool halted;
		static bool stopped;
};

#endif
