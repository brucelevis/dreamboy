/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "includes/bios.h"
#include "includes/log.h"
#include "includes/memory.h"

// init vars
const char *Bios::filename = NULL;

// responsible for loading the bios
bool Bios::Load(const char *filePath)
{
	bool result = false;
	FILE *biosRom = fopen(filePath, "rb");

	if (biosRom)
	{
		result = true;
		filename = filePath;

		fread(&Memory::mem, 1, 0x100, biosRom);
		Log::Print("Loaded bios '%s' successfully", filePath);
	}
	else
	{
		Log::Critical("Failed to load bios at filepath: '%s'", filePath);
	}

	fclose(biosRom);

	return result;
}

// responsible for reloading the bios
void Bios::Reload()
{
	Load(filename);
}
