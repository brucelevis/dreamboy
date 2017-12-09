/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef FLAGS_H
#define FLAGS_H

// includes
#include "typedefs.h"

class Flags
{
	public:
		static u8 Get(u8 flag);
		static void Set(u8 flags);
		static void Clear(u8 flags);

	public:
		static const u8 z;
		static const u8 n;
		static const u8 h;
		static const u8 c;
		static const u8 all;
};

#endif
