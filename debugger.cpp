/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

// includes
#include "src/imgui/imgui.h"
#include "src/imgui/imgui_impl_sdl.h"
#include "src/imgui/imgui_custom_extensions.h"
#include "src/imgui/imgui_memory_editor.h"
#include "src/tinyfiledialogs/tinyfiledialogs.h"
#include "src/includes/debugger.h"
#include "src/includes/flags.h"
#include "src/includes/memory.h"
#include "src/includes/rom.h"

// init vars
bool Debugger::stepThrough = true;
bool Debugger::stopAtBreakpoint = false;
bool Debugger::debuggerActive = false;
u16 Debugger::breakpoint = 0x00;
static char breakpointBuffer[256];
static MemoryEditor memoryViewer;

// create a controls window
void Debugger::ControlsWindow(const char *title, int width, int height, int x, int y)
{
	ImGui::Begin(title, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));

	if (ImGui::Button("Step Forward", ImVec2(width - 16, 0)))
	{
		stepThrough = true;
		stopAtBreakpoint = false;
		Cpu::ExecuteOpcode();
	}

	if (ImGui::Button("Run (no break)", ImVec2(width - 16, 0)))
	{
		stepThrough = false;
	}

	if (ImGui::Button("Run To Break PT", ImVec2(width - 16, 0)))
	{
		ImGui::OpenPopup("Set Breakpoint");
	}

	if (ImGui::Button("Stop", ImVec2(width - 16, 0)))
	{
		stepThrough = true;
		stopAtBreakpoint = false;
	}

	if (ImGui::Button("Reset", ImVec2(width - 16, 0)))
	{
		Memory::Init();
		Rom::Reload();
		Cpu::Init();
		// TODO: reload bios here if previously loaded
	}

	if (ImGui::BeginPopupModal("Set Breakpoint", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::PushItemWidth(180);
		ImGui::Text("Run To PC:");
		ImGui::InputText("", breakpointBuffer, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);

		if (ImGui::IsItemHovered())
		{
			ImGui::SetKeyboardFocusHere();
		}

		if (ImGui::Button("Ok", ImVec2(80, 0)))
		{
			// convert the breakpoint buffer to a short
			breakpoint = (u16)strtol(breakpointBuffer, NULL, 16);

			if (strlen(breakpointBuffer) > 0)
			{
				stepThrough = false;
				stopAtBreakpoint = true;
				ImGui::CloseCurrentPopup();
			}
			else
			{
				ImGui::OpenPopup("Breakpoint Error");
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::BeginPopupModal("Breakpoint Error", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::SetWindowSize("Breakpoint Error", ImVec2(230, 210));
			ImGuiExtensions::TextWithColors("{FF0000}No Breakpoint Entered!");
			ImGui::TextWrapped("You must set a breakpoint before you can start execution.\n\nIf you don't want to use a breakpoint, just hit cancel instead.");
			ImGui::NewLine();

			if (ImGui::Button("Ok", ImVec2(200, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::EndPopup();
	}

	ImGuiExtensions::TextWithColors("  {FF0000}Ins Ran:"); ImGui::Indent(20.f); ImGui::Text("%d", Cpu::instructionsRan); ImGui::Unindent(20.f);
	ImGui::End();
}

// create a rom info window
void Debugger::RomInfoWindow(const char *title, int width, int height, int x, int y)
{
	ImGui::Begin(title, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));
	ImGuiExtensions::TextWithColors("{FF0000}Name:"); ImGui::SameLine();

	// print the rom name
	for (u16 i = Memory::Address::ROM_NAME_START; i < Memory::Address::ROM_NAME_END; i++)
	{
		if (Memory::ReadByte(i) != 0)
		{
			ImGui::Text("%c", Memory::ReadByte(i));
			ImGui::SameLine();
		}
	}

	ImGui::NewLine();

	// rom type
	ImGuiExtensions::TextWithColors("{FF0000}Type: {FFFFFF}%02x", Memory::ReadByte(Memory::Address::ROM_TYPE));
	// rom rom-size
	ImGuiExtensions::TextWithColors("{FF0000}Rom-Size: {FFFFFF}%02x", Memory::ReadByte(Memory::Address::ROM_SIZE));
	// rom ram size
	ImGuiExtensions::TextWithColors("{FF0000}Ram-Size: {FFFFFF}%02x", Memory::ReadByte(Memory::Address::ROM_RAM_SIZE));
	// rom file name + path
	ImGuiExtensions::TextWithColors("{FF0000}Filename:");
	ImGui::TextWrapped("%s", Rom::filename);
	ImGui::End();
}

// create a file info window
void Debugger::FileWindow(const char *title, int width, int height, int x, int y)
{
	ImGui::Begin(title, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));

	if (ImGui::Button("Open Rom", ImVec2(width - 16, 0)))
	{
		char const *validExtensions[4] = {"*.gb", "*.GB", "*.bin", "*.BIN"};
		const char *filename = tinyfd_openFileDialog("Select Rom", "", 4, validExtensions, NULL, 0);

		if (filename != NULL)
		{
			stepThrough = true;
			stopAtBreakpoint = false;

			Memory::Init();
			Rom::Load(filename);
			Cpu::Init();
			// TODO: reload bios here if previously loaded
		}
	}

	if (ImGui::Button("Save State", ImVec2(width - 16, 0)))
	{
		Cpu::SaveState();
	}

	if (ImGui::Button("Load State", ImVec2(width - 16, 0)))
	{
		Cpu::LoadState();
	}

	if (ImGui::Button("Hide Debugger", ImVec2(width - 16, 0)))
	{
		debuggerActive = false;
		// should scale the game to full screen here too...
	}

	ImGui::End();
}

// create a memory viewer window
void Debugger::MemoryViewerWindow(const char *title, int width, int height, int x, int y, u16 startAddr, u16 endAddr)
{
	ImGui::Begin(title, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));

	for (u16 i = endAddr; i >= startAddr; --i)
	{
		ImGuiExtensions::TextWithColors("{FF0000}%04X: {FFFFFF}%02X", i, Memory::ReadByte(i));
	}
	ImGui::End();
}

// create a memory editor window
void Debugger::MemoryEditorWindow(const char *title, int width, int height, int x, int y)
{
	ImGui::Begin(title);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));
	memoryViewer.DrawContents(Memory::mem, 0x10000, 0x0000);
	memoryViewer.GotoAddrAndHighlight(Cpu::pc.reg, Cpu::pc.reg);
	ImGui::End();
}

// create a register viewer window
void Debugger::RegisterViewer(const char *title, int width, int height, int x, int y)
{
	bool flagZ = Flags::Get(Flags::z);
	bool flagN = Flags::Get(Flags::n);
	bool flagH = Flags::Get(Flags::h);
	bool flagC = Flags::Get(Flags::c);

	ImGui::Begin(title, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));
	ImGuiExtensions::TextWithColors("{FF0000}AF: {FFFFFF}%04X", Cpu::af.reg); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}LCDC: {FFFFFF}%02X", Memory::ReadByte(Memory::Address::LCDC)); ImGui::SameLine(); ImGui::NewLine(); ImGui::Unindent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}BC: {FFFFFF}%04X", Cpu::bc.reg); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}STAT: {FFFFFF}%02X", Memory::ReadByte(Memory::Address::STAT)); ImGui::Unindent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}DE: {FFFFFF}%04X", Cpu::de.reg); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}LY:   {FFFFFF}%02X", Memory::ReadByte(Memory::Address::LY)); ImGui::Unindent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}HL: {FFFFFF}%04X", Cpu::hl.reg); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}IME:  {FFFFFF}%d", 0); ImGui::Unindent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}SP: {FFFFFF}%04X", Cpu::sp.reg); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}IE:   {FFFFFF}%02X", Memory::ReadByte(Memory::Address::IE)); ImGui::Unindent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}PC: {FFFFFF}%04X", Cpu::pc.reg); ImGui::SameLine(); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}IF:   {FFFFFF}%02X", Memory::ReadByte(Memory::Address::IF)); ImGui::Unindent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}TMA:  {FFFFFF}%02X", Memory::ReadByte(Memory::Address::TIMA)); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}DIV:  {FFFFFF}%02X", Memory::ReadByte(Memory::Address::DIV)); ImGui::Unindent(80.f);
	ImGui::Checkbox("Z", &flagZ); ImGui::SameLine();
	ImGui::Checkbox("N", &flagN); ImGui::SameLine();
	ImGui::Checkbox("H", &flagH);
	ImGui::Checkbox("C", &flagC);
	ImGui::End();
}
