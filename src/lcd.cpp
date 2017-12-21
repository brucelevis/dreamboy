/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include <SDL2/SDL_opengl.h>
#include "includes/bit.h"
#include "includes/interrupts.h"
#include "includes/lcd.h"
#include "includes/log.h"
#include "includes/memory.h"

// definitions
#define LCD_CLOCK_CYCLES 456
#define LY Memory::mem[Memory::Address::LY]
#define LYC Memory::ReadByte(Memory::Address::LYC)
#define LCDC Memory::mem[Memory::Address::LCDC]
#define STAT Memory::mem[Memory::Address::STAT]
#define BGP Memory::mem[Memory::Address::BGP]
#define OP0 Memory::mem[Memory::Address::OP0]
#define OP1 Memory::mem[Memory::Address::OP1]
#define SCY Memory::ReadByte(Memory::Address::SCY)
#define SCX Memory::ReadByte(Memory::Address::SCX)
#define WY Memory::ReadByte(Memory::Address::WY)
#define WX Memory::ReadByte(Memory::Address::WX)

// init vars
u8 Lcd::screen[144][160][3];
int Lcd::width = 160;
int Lcd::height = 144;
int Lcd::scanlineCounter = 0;
static GLuint texture;

// responsible for initializing the Lcd
void Lcd::Init()
{
	Reset();

	// setup opengl for the game window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_TEXTURE_2D);
	UpdateTexture();
}

// responsible for resetting the Lcd
void Lcd::Reset()
{
	scanlineCounter = 0;

	for (int y = 0; y < 144; y++)
	{
		for (int x = 0; x < 160; x++)
		{
			screen[y][x][0] = 155;
			screen[y][x][1] = 188;
			screen[y][x][2] = 15;
		}
	}
}

// responsible for determining if the Lcd display is enabled
bool Lcd::Enabled()
{
	return Bit::Get(LCDC, 7);
}

// responsible for updating the Lcd controller
void Lcd::Update(int cycles)
{
	SetStatus();

	if (!Enabled()) return;

	scanlineCounter += cycles;

	if (scanlineCounter >= LCD_CLOCK_CYCLES)
	{
		if (LY < 144)
		{
			DrawScanline();
		}
		else if (LY == 144)
		{
			UpdateTexture();
			Interrupts::Request(Interrupts::VBLANK);
		}
		else if (LY > 153)
		{
			LY = 0x00;
		}

		LY += 1;

		scanlineCounter -= LCD_CLOCK_CYCLES;
	}
}

// responsible for setting the Lcd mode
u8 Lcd::SetMode(u8 mode)
{
	switch(mode)
	{
		case 0: Bit::Clear(STAT, 0); Bit::Clear(STAT, 1); break;
		case 1: Bit::Set(STAT, 0); Bit::Clear(STAT, 1); break;
		case 2: Bit::Clear(STAT, 0); Bit::Set(STAT, 1); break;
		case 3: Bit::Set(STAT, 0); Bit::Set(STAT, 1); break;
	}

	return mode;
}

// responsible for setting the Lcd status
void Lcd::SetStatus()
{
	bool requestInterrupt = false;
	u8 currentMode = (STAT & 0x3);
	u8 nextMode = 0;
	u16 vblankRange = 144;
	u16 oamRange =  (LCD_CLOCK_CYCLES - 80);
	u16 dataToLcd =  (oamRange - 172);

	if (!Enabled())
	{
		nextMode = SetMode(1);
		STAT &= 0xF8;
		LY = 0x00;
		scanlineCounter = 0;
		return;
	}

	if (LY >= vblankRange)
	{
		nextMode = SetMode(1);
		requestInterrupt = Bit::Get(STAT, 4);
	}
	else
	{
		if (scanlineCounter >= oamRange)
		{
			nextMode = SetMode(2);
			requestInterrupt = Bit::Get(STAT, 5);
		}
		else if (scanlineCounter >= dataToLcd)
		{
			nextMode = SetMode(3);
		}
		else
		{
			nextMode = SetMode(0);
			requestInterrupt = Bit::Get(STAT, 3);
		}
	}

	if (requestInterrupt && (nextMode != currentMode))
	{
		Interrupts::Request(Interrupts::LCD);
	}

	Bit::Clear(STAT, 2);

	if (LY == LYC) Bit::Set(STAT, 2);
	if (Bit::Get(STAT, 2) && Bit::Get(STAT, 6)) Interrupts::Request(Interrupts::LCD);

	STAT |= 0x80;
}

// responsible for updating the screen texture
void Lcd::UpdateTexture()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 160, 144, 0, GL_RGB, GL_UNSIGNED_BYTE, screen);
}

// responsible for rendering the image to the screen
void Lcd::Render()
{
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(0, 1); glVertex2f(0, height);
	glTexCoord2f(1, 1); glVertex2f(width, height);
	glTexCoord2f(1, 0); glVertex2f(width, 0);
	glEnd();
}

// responsible for determining if the background is enabled
bool Lcd::IsBackgroundEnabled()
{
	return Bit::Get(LCDC, 0);
}

// responsible for determining if the window is enabled
bool Lcd::IsWindowEnabled()
{
	return Bit::Get(LCDC, 5);
}

// responsible for determining if sprites are enabled
bool Lcd::IsSpritesEnabled()
{
	return Bit::Get(LCDC, 1);
}

// responsible for getting a color from a palette
Lcd::Rgb Lcd::GetColor(u8 palette, u8 bit)
{
	struct Rgb rgb;
	u8 hi = ((bit << 1) + 1);
	u8 lo = (bit << 1);
	u8 color = ((Bit::Get(palette, hi) << 1) | (Bit::Get(palette, lo)));

	switch(color)
	{
		case 0: rgb = {155, 188, 15}; break; // 00 - white
		case 1: rgb = {139, 172, 15}; break; // 01 - light grey
		case 2: rgb = {48, 98, 48}; break; // 02 - dark grey
		case 3: rgb = {15, 56, 15}; break; // 03 - black
	}

	return rgb;
}

// responsible for drawing the current scanline
void Lcd::DrawScanline()
{
	DrawBackground();
	DrawSprites();
}

// responsible for drawing the background
void Lcd::DrawBackground()
{
	if (!IsBackgroundEnabled()) return;

	u16 tileData = Bit::Get(LCDC, 4) ? 0x8000 : 0x8800;
	u16 tileMemory = Bit::Get(LCDC, 3) ? 0x9C00 : 0x9800;
	bool unsignedTile = Bit::Get(LCDC, 4);

	for (u8 x = 0; x < 160; x++)
	{
		u8 yPos = (SCY + LY);
		u8 xPos = (SCX + x);

		if (IsWindowEnabled() && (LY >= WY))
		{
			tileMemory = Bit::Get(LCDC, 6) ? 0x9C00 : 0x9800;
			yPos = (WY + LY);
			xPos = (WX + x);
		}
		else
		{
			tileMemory = Bit::Get(LCDC, 3) ? 0x9C00 : 0x9800;
		}

		u16 tileCol = (xPos / 8);
		u16 tileRow = ((yPos / 8) * 32);
		u8 tileYLine = ((yPos % 8) * 2);
		u16 tileAddress = (tileMemory + tileCol + tileRow);
		s16 tileNum = (unsignedTile) ? (u8)Memory::ReadByte(tileAddress) : (s8)Memory::ReadByte(tileAddress);
		u16 tileLocation = (unsignedTile) ? (tileData +  (tileNum * 16)) : ((tileData) + ((tileNum  + 128) * 16));
		u8 pixelData1 = Memory::ReadByte(tileLocation + tileYLine);
		u8 pixelData2 = Memory::ReadByte(tileLocation + tileYLine + 1);
		u8 colorBit = (((xPos % 8) - 7) * -1);
		u8 colorNum = ((Bit::Get(pixelData2, colorBit) << 1) | (Bit::Get(pixelData1, colorBit)));
		Rgb pixelColor = GetColor(BGP, colorNum);

		screen[LY][x][0] = pixelColor.r;
		screen[LY][x][1] = pixelColor.g;
		screen[LY][x][2] = pixelColor.b;
	}
}

// responsible for drawing sprites
void Lcd::DrawSprites()
{
	if (!IsSpritesEnabled()) return;

	u16 spriteData = 0x8000;
	u16 spriteAttributeData = 0xFE00;
	u8 spriteWidth = 8;
	u8 spriteHeight = Bit::Get(LCDC, 2) ? 16 : 8;
	u8 spriteLimit = 40;

	for (u8 i = 0; i < spriteLimit; i++)
	{
		u8 index = (i * 4);
		u8 yPos = Memory::ReadByte(spriteAttributeData + index) - 16;
		u8 xPos = Memory::ReadByte(spriteAttributeData + index + 1) - 8;
		u8 patternNo = Memory::ReadByte(spriteAttributeData + index + 2);
		u8 flags = Memory::ReadByte(spriteAttributeData + index + 3);

		if (spriteHeight == 16) Bit::Clear(patternNo, 0);

		u8 priority = Bit::Get(flags, 7);
		u8 yFlip = Bit::Get(flags, 6);
		u8 xFlip = Bit::Get(flags, 5);
		u8 paletteNo = Bit::Get(flags, 4);
		u8 palette = (paletteNo == 1) ? OP1 : OP0;
		u8 line = ((LY - yPos) * 2);
		u8 pixelData1 = Memory::ReadByte(spriteData + (patternNo * 16) + line);
		u8 pixelData2 = Memory::ReadByte(spriteData + (patternNo * 16) + line + 1);

		if (LY >= yPos && LY < (yPos + spriteHeight))
		{
			for (int pixel = 7; pixel >= 0; pixel--)
			{
				int spriteXPos = (xPos + (pixel * -1) + 7);
				u8 colorNum = ((Bit::Get(pixelData2, pixel) << 1) | (Bit::Get(pixelData1, pixel)));
				Rgb pixelColor = GetColor(palette, colorNum);

				if (pixelColor.r == 155) continue;

				if (xFlip) spriteXPos = (xPos + pixel);
				bool isWhite = (screen[LY][spriteXPos][0] == 155);

				if ((priority == 0x00) || (priority == 0x01 && isWhite))
				{
					screen[LY][spriteXPos][0] = pixelColor.r;
					screen[LY][spriteXPos][1] = pixelColor.g;
					screen[LY][spriteXPos][2] = pixelColor.b;
				}
			}
		}
	}
}
