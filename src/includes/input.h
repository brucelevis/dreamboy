/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef INPUT_H
#define INPUT_H

// includes
#include <SDL2/SDL.h>
#include "typedefs.h"

class Input
{
	public:
		static void Init();
		static void HandleKeys(SDL_Event event);
		static u8 GetKey(u8 data);

	private:
		static void PressDirection(u8 bit, u8 keyType);
		static void PressButton(u8 bit, u8 keyType);
		static void ReleaseKey(u8 bit);

	private:
		static u8 buttons;
};

#endif
