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
		static bool Load(const char *filePath);
		static bool Select();
		static void Reload();
		static bool HasLoaded();
		static bool LoadRam(int num = 0);
		static void SaveRam(int num = 0);

	public:
		static u8 rom[0x4000 * 128];
		static u8 ram[0x2000 * 4];
		static u8 mbcType;
		static u8 romSize;
		static u8 ramSize;
		static u16 romBank;
		static u8 ramBank;
		static u8 currentMode;
		static bool hasBatteryBackup;
		static const char *filename;
		static char romName[256];
};

#endif
