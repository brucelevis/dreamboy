/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#include "src/includes/memory.h"
#include "src/includes/rom.h"

// init vars
const char *Rom::filename = NULL;

// responsible for loading a rom
bool Rom::Load(const char *filepath)
{
	bool result = false;
	FILE *gbRom = fopen(filepath, "rb");

	if (gbRom)
	{
		printf("loaded rom '%s' successfully\n", filepath);
		result = true;
		filename = filepath;

		fread(&Memory::mem, 1, 0x8000, gbRom);

		// print the rom name
		printf("Rom Name: ");
		for (u16 i = Memory::Address::ROM_NAME_START; i < Memory::Address::ROM_NAME_END; i++)
		{
			printf("%c", Memory::mem[i]);
		}
		printf("\n");

		// print the rom cartridge type
		printf("Rom Cartridge Type: %02x | Rom-Size: %02x | Ram-Size: %02x\n", Memory::mem[Memory::Address::ROM_TYPE], Memory::mem[Memory::Address::ROM_SIZE], Memory::mem[Memory::Address::ROM_RAM_SIZE]);
	}
	else
	{
		printf("FAILED TO LOAD rom '%s'\n", filename);
	}

	// close the rom
	fclose(gbRom);

	return result;
}

// responsible for reloading a previously loaded rom
void Rom::Reload()
{
	Load(filename);
}
