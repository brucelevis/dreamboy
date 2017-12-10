/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef LOG_H
#define LOG_H

// includes
#include "typedefs.h"

class Log
{
	public:
		static void Init();
		static void Close();
		static void Print(const char *fmt, ...);
		static void Critical(const char *fmt, ...);
		static void ToFile(const char *str);
};

#endif
