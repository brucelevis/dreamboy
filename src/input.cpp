/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "includes/bit.h"
#include "includes/cpu.h"
#include "includes/input.h"
#include "includes/interrupts.h"
#include "includes/log.h"
#include "includes/memory.h"

// definitions
#define P15 5 // buttons
#define P14 4 // directional keys
#define BTN_START 7
#define BTN_SELECT 6
#define BTN_B 5
#define BTN_A 4
#define DIR_DOWN 3
#define DIR_UP 2
#define DIR_LEFT 1
#define DIR_RIGHT 0

// init vars
u8 Input::buttons = 0xFF;

// responsible for initializing the input
void Input::Init()
{
	buttons = 0xFF;
}

// responsible for pressing a key
void Input::PressKey(u8 bit, u8 keyType)
{
	const bool wasSet = Bit::Get(buttons, bit);

	Cpu::stopped = false;
	Bit::Clear(buttons, bit);
	if (wasSet) Interrupts::Request(Interrupts::JOYPAD);
}

// responsible for releasing a key
void Input::ReleaseKey(u8 bit)
{
	Bit::Set(buttons, bit);
}

// responsible for retrieving the currently pressed key
u8 Input::GetKey(u8 data)
{
	// buttons
	if (!Bit::Get(data, P15)) return ((buttons >> 4) | (0xF0));
	// directional keys
	if (!Bit::Get(data, P14)) return (buttons & 0x0F);

	return 0xFF;
}

// responsible for handling key input
void Input::HandleKeys(SDL_Event event)
{
	switch(event.type)
	{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
				case SDLK_LEFT: PressKey(DIR_LEFT, P14); break;
				case SDLK_RIGHT: PressKey(DIR_RIGHT, P14); break;
				case SDLK_UP: PressKey(DIR_UP, P14); break;
				case SDLK_DOWN: PressKey(DIR_DOWN, P14); break;
				case SDLK_z: PressKey(BTN_B, P15); break;
				case SDLK_x: PressKey(BTN_A, P15); break;
				case SDLK_RSHIFT: PressKey(BTN_SELECT, P15); break;
				case SDLK_RETURN: PressKey(BTN_START, P15); break;
			}
		break;

		case SDL_KEYUP:
			switch(event.key.keysym.sym)
			{
				case SDLK_LEFT: ReleaseKey(DIR_LEFT); break;
				case SDLK_RIGHT: ReleaseKey(DIR_RIGHT); break;
				case SDLK_UP: ReleaseKey(DIR_UP); break;
				case SDLK_DOWN: ReleaseKey(DIR_DOWN); break;
				case SDLK_z: ReleaseKey(BTN_B); break;
				case SDLK_x: ReleaseKey(BTN_A); break;
				case SDLK_RSHIFT: ReleaseKey(BTN_SELECT); break;
				case SDLK_RETURN: ReleaseKey(BTN_START); break;
			}
		break;
	}
}
