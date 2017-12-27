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
		static void Init();
		static void ExecuteOpcode();
		static void Step();
		static bool LoadState(bool fromDebugger, unsigned int num = 0);
		static void SaveState(bool fromDebugger, unsigned int num = 0);

	private:
		static void ExecuteExtendedOpcode();

	public:
		union Register
		{
			u16 reg;
			struct
			{
				u8 lo, hi;
			};
		};

		static Register af;
		static Register bc;
		static Register de;
		static Register hl;
		static Register sp;
		static Register pc;
		static int cycles;
		static int instructionsRan;
		static int framerate;
		static bool halted;
		static bool stopped;
		static bool pendingInterrupt;
		static bool haltBug;
		static bool didLoadBios;
};

#endif
