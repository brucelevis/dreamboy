/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include <SDL2/SDL.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_custom_extensions.h"
#include "includes/cpu.h"
#include "includes/debugger.h"
#include "includes/memory.h"
#include "includes/log.h"
#include "includes/rom.h"
#include "includes/ui.h"

// init vars
char Ui::statusText[512] = {0};
char Ui::statusTextTitle[256] = {0};
const char *statusWindowTitle = "Status Overlay";
static const char *aboutPopupTitle = "About";
static int currentPopup = -1;
static bool showPopup = false;
static bool hideMainMenu = false;
static bool hideStatusWindow = true;
static int statusMessageHideTime = (3 * 1000);
static SDL_TimerID timerId;

// responsible for executing upon a timer callback
static Uint32 timerCallback(Uint32 interval, void* param)
{
	if (timerId != 0)
	{
		Ui::HideStatusWindow();
		SDL_RemoveTimer(timerId);
		timerId = 0;
	}

    return 0;
}

// responsible for displaying the status window
void Ui::StatusWindowOverlay()
{
	if (hideStatusWindow) return;

	ImGui::Begin(statusWindowTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing);
	ImGui::SetWindowSize(statusWindowTitle, ImVec2(640, 80));
	ImGui::SetWindowPos(statusWindowTitle, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(10, 123, 255));
	ImGui::TextWrapped("%s", statusTextTitle);
	ImGui::PopStyleColor(1);
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255, 242, 0));
	ImGui::TextWrapped("%s", statusText);
	ImGui::PopStyleColor(1);
	ImGui::End();
}

// responsible for hiding the status window
void Ui::ShowStatusWindow()
{
	if (timerId != 0) SDL_RemoveTimer(timerId);
	timerId = SDL_AddTimer(statusMessageHideTime, timerCallback, (void *)"!" );
	hideStatusWindow = false;
}

// responsible for showing the status window
void Ui::HideStatusWindow()
{
	hideStatusWindow = true;
}

// responsible for setting the status message
void Ui::SetStatusMessage(const char *title, const char *msg)
{
	memset(statusTextTitle, 0, sizeof(statusTextTitle));
	memset(statusText, 0, sizeof(statusText));
	sprintf(statusTextTitle, "%s", title);
	sprintf(statusText, "%s", msg);
	ShowStatusWindow();
}

// responsible for hiding the main menu bar
void Ui::HideMainMenuBar()
{
	hideMainMenu = true;
}

// responsible for showing the main menu bar
void Ui::ShowMainMenuBar()
{
	hideMainMenu = false;
}

// responsible for displaying the about popup window
void Ui::AboutPopupWindow()
{
	if (ImGui::BeginPopupModal(aboutPopupTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::SetWindowSize(aboutPopupTitle, ImVec2(400, 380));
		ImGui::Text("DreamBoy: A Nintendo GameBoy Emulator.");
		ImGui::Text("Written by Danny Glover.");
		ImGui::NewLine();
		ImGui::BulletText("Written in C++");
		ImGui::BulletText("Uses SDL/OGL for drawing");
		ImGui::BulletText("Uses ImGui for the debugger");
		ImGui::NewLine();
		ImGui::TextWrapped("GitHub:\nhttps://github.com/DannyGlover/dreamboy");
		ImGui::NewLine();
		ImGui::Text("Greetz:");
		ImGui::NewLine();
		ImGui::BulletText("Izik, Mehcode, Gekkio, GhostSonic, Stian");
		ImGui::BulletText("Emulation Development Slack Channel");
		ImGui::NewLine();

		if (ImGui::Button("Close", ImVec2(385, 0))) ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

// responsible for displaying the UI
void Ui::Render()
{
	StatusWindowOverlay();

	if (hideMainMenu) return;

	if (ImGui::BeginMainMenuBar())
	{
		// file menu
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Rom"))
			{
				if (ImGui::MenuItem("Open", "ctrl+o")) Debugger::SelectRom();
				if (ImGui::MenuItem("Close", "ctrl+c")) Debugger::ResetSystem();
				if (ImGui::MenuItem("Info"))
				{
					currentPopup = RomInfoPopup;
					showPopup = true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("State"))
			{
				if (ImGui::MenuItem("Load", "ctrl+l"))
				{
					if (!Cpu::LoadState(false))
					{
						const char *title = "Failed To Load State";
						const char *desc = "The state could not be found";
						SetStatusMessage(title, desc);
					}
				}
				if (ImGui::MenuItem("Save", "ctrl+s")) Cpu::SaveState(false);
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Save Screenshot", "ctrl+p")) Debugger::SaveScreenshot();

			if (ImGui::MenuItem("Quit", "ctrl+q"))
			{
				Rom::SaveRam();
				Debugger::RemoveStates();
				exit(0);
			}

			ImGui::EndMenu();
		}

		// game menu
		if (ImGui::BeginMenu("Game"))
		{
			if (ImGui::MenuItem("Run"))
			{
				Debugger::stepThrough = false;
				hideMainMenu = true;
			}
			if (ImGui::MenuItem("Stop")) Debugger::stepThrough = true;

			if (ImGui::BeginMenu("Speed"))
			{
				int framerate = 60;
				if (ImGui::MenuItem("1x (normal)")) Cpu::framerate = framerate;
				if (ImGui::MenuItem("2x")) Cpu::framerate = (framerate / 2);
				if (ImGui::MenuItem("3x")) Cpu::framerate = (framerate / 4);
				if (ImGui::MenuItem("4x")) Cpu::framerate = (framerate / 4);
				if (ImGui::MenuItem("5x")) Cpu::framerate = (framerate / 5);
				if (ImGui::MenuItem("6x")) Cpu::framerate = (framerate / 6);
				if (ImGui::MenuItem("7x")) Cpu::framerate = (framerate / 7);
				if (ImGui::MenuItem("8x")) Cpu::framerate = (framerate / 8);
				if (ImGui::MenuItem("9x")) Cpu::framerate = (framerate / 9);
				if (ImGui::MenuItem("10x")) Cpu::framerate = (framerate / 10);
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		// debugger menu
		if (ImGui::BeginMenu("Debugger"))
		{
			// memory menu
			if (ImGui::BeginMenu("Memory"))
			{
				if (ImGui::MenuItem("View"))
				{
					currentPopup = MemoryViewPopup;
					showPopup = true;
				}

				if (ImGui::MenuItem("Get Value"))
				{
					currentPopup = MemoryGetValuePopup;
					showPopup = true;
				}

				if (ImGui::MenuItem("Set Value"))
				{
					currentPopup = MemorySetValuePopup;
					showPopup = true;
				}

				if (ImGui::MenuItem("Dump")) Debugger::DumpMemory();
				if (ImGui::MenuItem("Import")) Debugger::ImportMemory();

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Modify Registers/Flags", "ctrl+m"))
			{
				currentPopup = ModifyRegistersPopup;
				showPopup = true;
			}

			// step menu
			if (ImGui::BeginMenu("Step Mode"))
			{
				if (ImGui::MenuItem("Enable")) Debugger::stepThrough = true;
				if (ImGui::MenuItem("Disable")) Debugger::stepThrough = false;
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Step Forward", "ctrl+f"))
			{
				Cpu::Step();
				Cpu::SaveState(true, Cpu::instructionsRan);
			}

			if (ImGui::MenuItem("Step Backward", "ctrl+b"))
			{
				if (Debugger::stepThrough)
				{
					if (Cpu::instructionsRan > 0)
					{
						Cpu::LoadState(true, Cpu::instructionsRan);
						Cpu::instructionsRan -= 1;
					}
					else
					{
						Debugger::ResetSystem();
					}
				}
			}

			if (ImGui::MenuItem("Run (No Break)", "ctrl+r"))
			{
				Debugger::stepThrough = false;
				Debugger::stopAtBreakpoint = false;
			}

			if (ImGui::MenuItem("Run To Break Point"))
			{
				currentPopup = SetBreakpointPopup;
				showPopup = true;
			}

			if (ImGui::MenuItem("Reset"))
			{
				Debugger::stepThrough = true;
				Debugger::stopAtBreakpoint = false;
				Debugger::ResetSystem();
			}

			if (ImGui::MenuItem("Stop"))
			{
				Debugger::stepThrough = true;
				Debugger::stopAtBreakpoint = false;
			}
			ImGui::EndMenu();
		}

		// help menu
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				currentPopup = AboutPopup;
				showPopup = true;
			}
			ImGui::EndMenu();
		}

		switch(currentPopup)
		{
			case AboutPopup:
				if (showPopup) ImGui::OpenPopup(aboutPopupTitle);
				AboutPopupWindow();
				showPopup = false;
			break;

			case RomInfoPopup:
				if (showPopup) ImGui::OpenPopup(Debugger::romInfoPopupTitle);
				Debugger::RomInfoPopup();
				showPopup = false;
			break;

			case MemoryViewPopup:
				if (showPopup) ImGui::OpenPopup(Debugger::memViewPopupTitle);
				Debugger::ViewMemoryPopup();
				showPopup = false;
			break;

			case MemoryGetValuePopup:
				if (showPopup) ImGui::OpenPopup(Debugger::memGetValuePopupTitle);
				Debugger::GetMemoryValuePopup();
				showPopup = false;
			break;

			case MemorySetValuePopup:
				if (showPopup) ImGui::OpenPopup(Debugger::memSetValuePopupTitle);
				Debugger::SetMemoryValuePopup();
				showPopup = false;
			break;

			case ModifyRegistersPopup:
				if (showPopup) ImGui::OpenPopup(Debugger::modifyRegistersPopupTitle);
				Debugger::ModifyRegistersPopup();
				showPopup = false;
			break;

			case SetBreakpointPopup:
				if (showPopup) ImGui::OpenPopup(Debugger::setBreakpointPopupTitle);
				Debugger::SetBreakpointPopup();
				showPopup = false;
			break;
		}

		ImGui::EndMainMenuBar();
	}
}
