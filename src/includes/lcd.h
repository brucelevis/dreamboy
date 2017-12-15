/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef LCD_H
#define LCD_H

// includes
#include "typedefs.h"

class Lcd
{
	public:
		static void Init();
		static void Reset();
		static bool Enabled();
		static void Update(int cycles);
		static void Render();

	private:
		struct Rgb
		{
			u8 r, g, b;
		};

		static u8 SetMode(u8 mode);
		static void SetStatus();
		static bool IsBackgroundEnabled();
		static bool IsWindowEnabled();
		static bool IsSpritesEnabled();
		static void UpdateTexture();
		static Rgb GetColor(u8 palette, u8 bit);
		static void DrawScanline();
		static void DrawBackground();
		static void DrawWindow();
		static void DrawSprites();

	public:
		static int width;
		static int height;
		static u8 screen[144][160][3];

	private:
		static int scanlineCounter;
};

#endif
