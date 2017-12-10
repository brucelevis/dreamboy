/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef LCD_H
#define LCD_H

// includes
#include "typedefs.h"

class Lcd
{
	public:
		static void Init();
		static void Reset();
		static bool Enabled();
		static void Update(int cycles);

	private:
		static int scanlineCounter;
};

#endif
