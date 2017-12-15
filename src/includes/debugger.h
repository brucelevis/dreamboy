/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef DEBUGGER_H
#define DEBUGGER_H

// includes
#include "typedefs.h"

// definitions
#define USE_DEBUGGER 1

class Debugger
{
	public:
		static void ResetSystem(const char *newRomFilename = NULL);
		static void SaveScreenshot();
		static void ShowDebugger();
		static void HideDebugger();
		static void SelectRom();
		static void ControlsWindow(const char *title, int width, int height, int x, int y);
		static void RomInfoWindow(const char *title, int width, int height, int x, int y);
		static void FileWindow(const char *title, int width, int height, int x, int y);
		static void MemoryViewerWindow(const char *title, int width, int height, int x, int y, u16 startAddr, u16 endAddr);
		static void MemoryEditorWindow(const char *title, int width, int height, int x, int y);
		static void RegisterViewer(const char *title, int width, int height, int x, int y);

	public:
		static bool stepThrough;
		static bool stopAtBreakpoint;
		static bool active;
		static u16 breakpoint;
};

#endif
