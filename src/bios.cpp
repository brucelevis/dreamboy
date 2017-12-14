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
		printf("loaded bios '%s' successfully\n", filePath);
		result = true;
		filename = filePath;

		fread(&Memory::mem, 1, 0x100, biosRom);
	}
	else
	{
		printf("FAILED TO LOAD bios '%s'\n", filename);
	}

	// close the bios
	fclose(biosRom);

	return result;
}

// responsible for reloading the bios
void Bios::Reload()
{
	Load(filename);
}
