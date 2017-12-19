/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

// includes
#include "typedefs.h"

class Interrupts
{
	public:
		static void Init();
		static void Request(int id);
		static void Service();

	private:
		static bool IsRequested(int id);
		static bool IsEnabled(int id);
		static void Reset(int id);
		static int RequestedId();

	public:
		enum
		{
			VBLANK, LCD, TIMER, SERIAL, JOYPAD
		};
		static bool ime;
		static bool clearIF;
		static bool shouldExecute;
		static u8 pendingCount;
};

#endif
