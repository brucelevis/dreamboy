/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#ifndef UI_H
#define UI_H

// includes
#include "typedefs.h"

class Ui
{
	public:
		static void Render();
		static void SetStatusMessage(const char *title, const char *message);
		static void HideStatusWindow();
		static void ShowStatusWindow();
		static void HideMainMenuBar();
		static void ShowMainMenuBar();

	private:
		static void StatusWindowOverlay();
		static void AboutPopupWindow();

	public:
		enum
		{
			AboutPopup, RomInfoPopup, MemoryViewPopup, MemoryGetValuePopup,
			MemorySetValuePopup, ModifyRegistersPopup, SetBreakpointPopup,
		};

	private:
		static char statusText[512];
		static char statusTextTitle[256];
};

#endif
