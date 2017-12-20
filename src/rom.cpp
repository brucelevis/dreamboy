/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#include "includes/memory.h"
#include "includes/log.h"
#include "includes/rom.h"

// init vars
u8 Rom::rom[0x4000 * 128] = {0x00};
u8 Rom::ram[0x2000 * 4] = {0x00};
u8 Rom::mbcType = 0x00;
u8 Rom::romSize = 0x00;
u8 Rom::ramSize = 0x00;
u8 Rom::romBank = 0x01;
u16 Rom::ramBank = 0x0000;
u8 Rom::currentMode = 0x00;
const char *Rom::filename = NULL;

// responsible for loading a rom
bool Rom::Load(const char *filePath)
{
	bool result = false;
	FILE *gbRom = fopen(filePath, "rb");
	romBank = 0x01;
	ramSize = 0x00;
	currentMode = 0x00;

	if (gbRom)
	{
		memset(&rom, 0x00, sizeof(rom));
		memset(&ram, 0x00, sizeof(ram));
		fread(&rom, 1, sizeof(rom), gbRom);
		memcpy(&Memory::mem, &rom, 0x3FFF);

		result = true;
		filename = filePath;
		mbcType = Memory::ReadByte(Memory::Address::ROM_TYPE);
		romSize = Memory::ReadByte(Memory::Address::ROM_SIZE);
		ramSize = Memory::ReadByte(Memory::Address::ROM_RAM_SIZE);

		Log::Print("Loaded rom '%s' successfully", filePath);
		printf("Rom Name: ");

		for (u16 i = Memory::Address::ROM_NAME_START; i < Memory::Address::ROM_NAME_END; i++)
		{
			printf("%c", Memory::ReadByte(i));
		}

		printf("\n");
		Log::Print("Rom Cartridge Type: %02X | Rom-Size: %02X | Ram-Size: %02X", mbcType, romSize, ramSize);

		LoadRam();
	}
	else
	{
		Log::Critical("Failed to load rom at filepath: '%s'", filePath);
	}

	fclose(gbRom);

	return result;
}

// responsible for reloading a previously loaded rom
void Rom::Reload()
{
	Load(filename);
}

// responsible for loading the games ram bank from a file
bool Rom::LoadRam(int num)
{
	char outputFilename[512];
	char romName[512];
	char filePath[512];
	const char *cleanFilename;
	(cleanFilename = strrchr(filename, '/')) ? ++filename : (cleanFilename = filename);

	// get the current file extension and rename it to sav
	sscanf(cleanFilename,"%[^.]", romName);
	sprintf(romName,"%s", romName);
	sprintf(filePath, "saves/%s", romName);
	sprintf(outputFilename, "%s/%d.sav", filePath, num);

	FILE *fp = fopen(outputFilename, "rb");

	if (fp == NULL) return false;

	fread(&Rom::ram, 1, sizeof(Rom::ram), fp);
	fclose(fp);

	return true;
}


// responsible for saving the games ram bank to a file
void Rom::SaveRam(int num)
{
	struct stat st = {0};
	char outputFilename[512];
	char romName[512];
	char filePath[512];
	const char *cleanFilename;
	(cleanFilename = strrchr(filename, '/')) ? ++filename : (cleanFilename = filename);

	// get the current file extension and rename it to sav
	sscanf(cleanFilename,"%[^.]", romName);
	sprintf(romName,"%s", romName);
	sprintf(filePath, "saves/%s", romName);

	if (stat(filePath, &st) == -1) mkdir(filePath, 0700);

	sprintf(outputFilename, "%s/%d.sav", filePath, num);

	FILE *fp = fopen(outputFilename, "wb");

	fwrite(Rom::ram, sizeof(Rom::ram), 1, fp);
	fclose(fp);
}
