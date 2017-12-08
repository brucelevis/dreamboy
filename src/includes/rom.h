/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef ROM_H
#define ROM_H

// includes
#include "typedefs.h"

class Rom
{
	public:
		static bool Load(const char *filepath);
		static void Reload();
		static const char *filename;
};

#endif
