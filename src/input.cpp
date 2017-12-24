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
#define JOYSTICK_DEAD_ZONE 8000

// init vars
u8 Input::buttons = 0xFF;
static SDL_GameController *gamePad;

// responsible for handling added controllers
static void ControllerAdded(int id)
{
	if (SDL_IsGameController(id))
	{
		gamePad = SDL_GameControllerOpen(id);

		// todo: user configurable input
		/*
		if (pad != NULL)
		{
			//SDL_Joystick *joy = SDL_GameControllerGetJoystick(pad);
			//int instanceID = SDL_JoystickInstanceID(joy);
		}*/
	}
}

// responsible for initializing the input
void Input::Init()
{
	buttons = 0xFF;
}

// responsible for pressing a directional key
void Input::PressDirection(u8 bit, u8 keyType)
{
	const bool wasNotSet = Bit::Get(buttons, bit);

	switch(bit)
	{
		case DIR_DOWN: ReleaseKey(DIR_UP); break;
		case DIR_UP: ReleaseKey(DIR_DOWN); break;
		case DIR_LEFT: ReleaseKey(DIR_RIGHT); break;
		case DIR_RIGHT: ReleaseKey(DIR_LEFT); break;
	}

	Cpu::stopped = false;
	Bit::Clear(buttons, bit);

	if (wasNotSet) Interrupts::Request(Interrupts::JOYPAD);
}

// responsible for pressing a button key
void Input::PressButton(u8 bit, u8 keyType)
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
		case SDL_CONTROLLERDEVICEADDED:
			ControllerAdded(event.cdevice.which);
		break;

		case SDL_JOYAXISMOTION:
			if (event.jaxis.which == 0)
			{
				// x axis
				if (event.jaxis.axis == 0)
				{
					// left
					if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						PressDirection(DIR_LEFT, P14);
					}
					// right
					else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						PressDirection(DIR_RIGHT, P14);
					}
					else
					{
						ReleaseKey(DIR_LEFT);
						ReleaseKey(DIR_RIGHT);
					}
				}
				// y axis
				else if (event.jaxis.axis == 1)
				{
					// up
					if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
					{
						PressDirection(DIR_UP, P14);
					}
					// down
					else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
					{
						PressDirection(DIR_DOWN, P14);
					}
					else
					{
						ReleaseKey(DIR_UP);
						ReleaseKey(DIR_DOWN);
					}
				}
			}
		break;

		case SDL_CONTROLLERBUTTONDOWN:
			switch(event.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT: PressDirection(DIR_LEFT, P14); break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: PressDirection(DIR_RIGHT, P14); break;
				case SDL_CONTROLLER_BUTTON_DPAD_UP: PressDirection(DIR_UP, P14); break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN: PressDirection(DIR_DOWN, P14); break;
				case SDL_CONTROLLER_BUTTON_A: PressButton(BTN_B, P15); break;
				case SDL_CONTROLLER_BUTTON_B: PressButton(BTN_A, P15); break;
				case SDL_CONTROLLER_BUTTON_BACK: PressButton(BTN_SELECT, P15); break;
				case SDL_CONTROLLER_BUTTON_START: PressButton(BTN_START, P15); break;
			}
		break;

		case SDL_CONTROLLERBUTTONUP:
			switch(event.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT: ReleaseKey(DIR_LEFT); break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: ReleaseKey(DIR_RIGHT); break;
				case SDL_CONTROLLER_BUTTON_DPAD_UP: ReleaseKey(DIR_UP); break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN: ReleaseKey(DIR_DOWN); break;
				case SDL_CONTROLLER_BUTTON_A: ReleaseKey(BTN_B); break;
				case SDL_CONTROLLER_BUTTON_B: ReleaseKey(BTN_A); break;
				case SDL_CONTROLLER_BUTTON_BACK: ReleaseKey(BTN_SELECT); break;
				case SDL_CONTROLLER_BUTTON_START: ReleaseKey(BTN_START); break;
			}
		break;

		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
				case SDLK_LEFT: PressDirection(DIR_LEFT, P14); break;
				case SDLK_RIGHT: PressDirection(DIR_RIGHT, P14); break;
				case SDLK_UP: PressDirection(DIR_UP, P14); break;
				case SDLK_DOWN: PressDirection(DIR_DOWN, P14); break;
				case SDLK_z: PressButton(BTN_B, P15); break;
				case SDLK_x: PressButton(BTN_A, P15); break;
				case SDLK_RSHIFT: PressButton(BTN_SELECT, P15); break;
				case SDLK_RETURN: PressButton(BTN_START, P15); break;
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
