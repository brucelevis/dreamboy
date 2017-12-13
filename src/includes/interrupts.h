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
		static void Request(u8 id);
		static void Service();

	private:
		static bool IsRequested(u8 id);
		static bool IsEnabled(u8 id);
		static void Reset(u8 id);
		static u8 RequestedId();

	public:
		enum class ID
		{
			VBLANK, STAT, TIMER, SERIAL, JOYPAD
		};
		static bool ime;
		static u8 pendingCount;
};

#endif
