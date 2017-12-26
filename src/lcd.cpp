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
int Lcd::height = 480;
int Lcd::width = 640;
int Lcd::scanlineCounter = 0;
static const Lcd::Rgb colorPalette[4] =
{
	{155, 188, 15}, {139, 172, 15}, {48, 98, 48}, {15, 56, 15}
};
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
			screen[y][x][0] = 0;
			screen[y][x][1] = 0;
			screen[y][x][2] = 0;
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
		switch(LY)
		{
			case 0 ... 143: DrawScanline(); break;

			case 144:
				UpdateTexture();
				Interrupts::Request(Interrupts::VBLANK);
			break;

			case 154: LY = 0xFF; break;
		}

		LY += 1;

		if (LY == LYC) Bit::Set(STAT, 2); else Bit::Clear(STAT, 2);
		if (Bit::Get(STAT, 2) && Bit::Get(STAT, 6)) Interrupts::Request(Interrupts::LCD);

		STAT |= 0x80;
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

	STAT |= 0x80;

	return mode;
}

// responsible for setting the Lcd status
void Lcd::SetStatus()
{
	bool requestInterrupt = false;
	u8 nextMode = 0;
	const u8 currentMode = (STAT & 0x3);
	const u16 vblankRange = 144;
	const u16 oamRange = (LCD_CLOCK_CYCLES - 80);
	const u16 dataToLcd = (oamRange - 172);

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
		switch(scanlineCounter)
		{
			case oamRange ... LCD_CLOCK_CYCLES:
				nextMode = SetMode(2);
				requestInterrupt = Bit::Get(STAT, 5);
			break;

			case dataToLcd ... (oamRange - 1):
				nextMode = SetMode(3);
			break;

			default:
				nextMode = SetMode(0);
				requestInterrupt = Bit::Get(STAT, 3);
			break;
		}
	}

	if (requestInterrupt && (nextMode != currentMode))
	{
		Interrupts::Request(Interrupts::LCD);
	}
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
	const u8 hi = ((bit << 1) + 1);
	const u8 lo = (bit << 1);
	const u8 color = ((Bit::Get(palette, hi) << 1) | (Bit::Get(palette, lo)));

	return colorPalette[color];
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

	u16 tileMemory = Bit::Get(LCDC, 3) ? 0x9C00 : 0x9800;
	const u16 tileData = Bit::Get(LCDC, 4) ? 0x8000 : 0x8800;
	const u16 tileMemoryAddress = tileMemory;
	const u16 windowMemoryAddress = Bit::Get(LCDC, 6) ? 0x9C00 : 0x9800;
	const bool unsignedTile = Bit::Get(LCDC, 4);
	const bool windowEnabled = IsWindowEnabled();

	for (u8 x = 0; x < 160; x++)
	{
		u8 yPos = (SCY + LY);
		u8 xPos = (SCX + x);

		if (windowEnabled && (LY >= WY) && (x >= (WX - 7)))
		{
			tileMemory = windowMemoryAddress;
			yPos = (LY - WY);
			xPos = ((WX - 7) + x);
		}
		else
		{
			tileMemory = tileMemoryAddress;
		}

		const u16 tileCol = (xPos / 8);
		const u16 tileRow = ((yPos / 8) * 32);
		const u8 tileYLine = ((yPos % 8) * 2);
		const u16 tileAddress = (tileMemory + tileCol + tileRow);
		const s16 tileNum = (unsignedTile) ? (u8)Memory::ReadByte(tileAddress) : (s8)Memory::ReadByte(tileAddress);
		const u16 tileLocation = (unsignedTile) ? (tileData +  (tileNum * 16)) : ((tileData) + ((tileNum  + 128) * 16));
		const u8 pixelData1 = Memory::ReadByte(tileLocation + tileYLine);
		const u8 pixelData2 = Memory::ReadByte(tileLocation + tileYLine + 1);
		const u8 colorBit = (((xPos % 8) - 7) * -1);
		const u8 colorNum = ((Bit::Get(pixelData2, colorBit) << 1) | (Bit::Get(pixelData1, colorBit)));
		const Rgb pixelColor = GetColor(BGP, colorNum);

		screen[LY][x][0] = pixelColor.r;
		screen[LY][x][1] = pixelColor.g;
		screen[LY][x][2] = pixelColor.b;
	}
}

// responsible for drawing sprites
void Lcd::DrawSprites()
{
	if (!IsSpritesEnabled()) return;

	const u16 spriteData = 0x8000;
	const u16 spriteAttributeData = 0xFE00;
	const u8 spriteWidth = 8;
	const u8 spriteHeight = Bit::Get(LCDC, 2) ? 16 : 8;
	const u8 spriteLimit = 40;

	for (int i = (spriteLimit - 1); i >= 0; i--)
	{
		const u8 index = (i * 4);
		const u8 yPos = Memory::ReadByte(spriteAttributeData + index) - 16;
		const u8 xPos = Memory::ReadByte(spriteAttributeData + index + 1) - 8;
		u8 patternNo = Memory::ReadByte(spriteAttributeData + index + 2);
		const u8 flags = Memory::ReadByte(spriteAttributeData + index + 3);

		if (spriteHeight == 16) Bit::Clear(patternNo, 0);

		const u8 priority = Bit::Get(flags, 7);
		const u8 yFlip = Bit::Get(flags, 6);
		const u8 xFlip = Bit::Get(flags, 5);
		const u8 palette = (Bit::Get(flags, 4)) ? OP1 : OP0;
		const u8 line = (yFlip) ? ((((LY - yPos - spriteHeight) + 1) * -1) * 2) : ((LY - yPos) * 2);
		const u8 pixelData1 = Memory::ReadByte(spriteData + (patternNo * 16) + line);
		const u8 pixelData2 = Memory::ReadByte(spriteData + (patternNo * 16) + line + 1);

		// sprites at position 0 are not drawn
		if (xPos == 0 && yPos == 0) continue;

		if (LY >= yPos && LY < (yPos + spriteHeight))
		{
			for (int pixel = 7; pixel >= 0; pixel--)
			{
				const u8 x = (xPos + pixel);
				const int spritePixel = (xFlip) ? pixel : ((pixel - 7) * -1);
				const bool isWhite = (screen[LY][x][0] == 155);
				const u8 colorNum = ((Bit::Get(pixelData2, spritePixel) << 1) | (Bit::Get(pixelData1, spritePixel)));
				const Rgb pixelColor = GetColor(palette, colorNum);

				// skip drawing off-screen sprites
				if (x >= 160) continue;
				// skip drawing transparent pixels
				if (colorNum == 0x0) continue;
				// with priority 0x1, if the background pixel isn't white, the sprite isn't drawn
				if (priority == 0x1 && !isWhite) continue;

				screen[LY][x][0] = pixelColor.r;
				screen[LY][x][1] = pixelColor.g;
				screen[LY][x][2] = pixelColor.b;
			}
		}
	}
}
