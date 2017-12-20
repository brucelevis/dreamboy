/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "typedefs.h"

class Timer
{
	public:
		static void Init();
		static u16 GetFrequency();
		static bool Enabled();
		static void Update(int cycles);

	public:
		static int timerCounter;
		static int divCounter;

	private:
		static void UpdateDiv(int cycles);
};
