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
static char regBuffer[256];
static MemoryEditor memoryViewer;
static  enum
{
	AF, BC, DE, HL, SP, PC, LCDC, STAT, LY, IME, IF, IE, TIMA, TAC, TMA, DIV, Z, N, H, C
} reg;

// responsible for resetting the system
void Debugger::ResetSystem(const char *newRomFilename)
{
	Memory::Init();
	if (newRomFilename != NULL) Rom::Load(newRomFilename); else Rom::Reload();
	Cpu::Init();
}

// create a controls window
void Debugger::ControlsWindow(const char *title, int width, int height, int x, int y)
{
	const char *modifyRegTitle = "Modify Reg/Flags";
	const char *setRegTitle = "Set Register";
	const char *setBreakPtTitle = "Set Breakpoint";
	const char *breakPtErrorTitle = "Breakpoint Error";

	ImGui::Begin(title, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(25, 0, 51));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(102, 102, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0, 0, 204));

	if (ImGui::Button(modifyRegTitle, ImVec2(width - 16, 0)))
	{
		sprintf(regBuffer, "");
		ImGui::OpenPopup(modifyRegTitle);
	}

	if (ImGui::Button("Step Forward", ImVec2(width - 16, 0)))
	{
		stepThrough = true;
		stopAtBreakpoint = false;
		Cpu::ExecuteOpcode();
	}

	if (ImGui::Button("Step Backward", ImVec2(width - 16, 0)))
	{
		stepThrough = true;
		stopAtBreakpoint = false;

		if (Cpu::instructionsRan > 0)
		{
			Cpu::LoadState(Cpu::instructionsRan);
			Cpu::instructionsRan -= 1;
		}
		else
		{
			ResetSystem();
		}
	}

	if (ImGui::Button("Run (no break)", ImVec2(width - 16, 0)))
	{
		stepThrough = false;
		stopAtBreakpoint = false;
	}

	if (ImGui::Button("Run To Break PT", ImVec2(width - 16, 0)))
	{
		ImGui::OpenPopup(setBreakPtTitle);
	}

	if (ImGui::Button("Stop", ImVec2(width - 16, 0)))
	{
		stepThrough = true;
		stopAtBreakpoint = false;
	}

	if (ImGui::Button("Reset", ImVec2(width - 16, 0)))
	{
		ResetSystem();
	}

	ImGui::PopStyleColor(3);

	if (ImGui::BeginPopupModal(modifyRegTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::SetWindowSize(modifyRegTitle, ImVec2(280, 310));
		ImGui::Text("Choose The Register To Modify:");
		ImGui::NewLine();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(25, 0, 51));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(102, 102, 255));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0, 0, 204));

		// block 1
		if (ImGui::Button("AF", ImVec2(60, 0)))
		{
			reg = AF;
			sprintf(regBuffer, "%04X", Cpu::af.reg);
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("BC", ImVec2(60, 0)))
		{
			reg = BC;
			sprintf(regBuffer, "%04X", Cpu::bc.reg);
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("DE", ImVec2(60, 0)))
		{
			reg = DE;
			sprintf(regBuffer, "%04X", Cpu::de.reg);
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("HL", ImVec2(60, 0)))
		{
			reg = HL;
			sprintf(regBuffer, "%04X", Cpu::hl.reg);
			ImGui::OpenPopup(setRegTitle);
		}

		// block 2
		if (ImGui::Button("SP", ImVec2(60, 0)))
		{
			reg = SP;
			sprintf(regBuffer, "%04X", Cpu::sp.reg);
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("PC", ImVec2(60, 0)))
		{
			reg = PC;
			sprintf(regBuffer, "%04X", Cpu::pc.reg);
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("LCDC", ImVec2(60, 0)))
		{
			reg = LCDC;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::LCDC));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("STAT", ImVec2(60, 0)))
		{
			reg = STAT;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::STAT));
			ImGui::OpenPopup(setRegTitle);
		}

		// block 3
		if (ImGui::Button("LY", ImVec2(60, 0)))
		{
			reg = LY;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::LY));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("IME", ImVec2(60, 0)))
		{
			reg = IME;
			//sprintf(regBuffer, "%d", ime);
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("IF", ImVec2(60, 0)))
		{
			reg = IF;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::IF));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("IE", ImVec2(60, 0)))
		{
			reg = IE;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::IE));
			ImGui::OpenPopup(setRegTitle);
		}

		// block 4
		if (ImGui::Button("TIMA", ImVec2(60, 0)))
		{
			reg = TIMA;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::TIMA));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("TAC", ImVec2(60, 0)))
		{
			reg = TAC;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::TAC));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("TMA", ImVec2(60, 0)))
		{
			reg = TMA;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::TMA));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("DIV", ImVec2(60, 0)))
		{
			reg = DIV;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::DIV));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::NewLine();
		ImGui::Text("Choose A Flag To Modify:");
		ImGui::NewLine();

		// block 5
		if (ImGui::Button("Z", ImVec2(60, 0)))
		{
			reg = Z;
			sprintf(regBuffer, "%d", Flags::Get(Flags::z));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("N", ImVec2(60, 0)))
		{
			reg = N;
			sprintf(regBuffer, "%d", Flags::Get(Flags::n));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("H", ImVec2(60, 0)))
		{
			reg = H;
			sprintf(regBuffer, "%d", Flags::Get(Flags::h));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::SameLine();

		if (ImGui::Button("C", ImVec2(60, 0)))
		{
			reg = C;
			sprintf(regBuffer, "%d", Flags::Get(Flags::c));
			ImGui::OpenPopup(setRegTitle);
		}

		ImGui::PopStyleColor(3);
		ImGui::NewLine();
		ImGui::Indent(60.f);

		if (ImGui::Button("Close", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::BeginPopupModal(setRegTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::PushItemWidth(180);
			ImGui::Text("New Value:");
			ImGui::InputText("", regBuffer, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);

			if (ImGui::IsItemHovered())
			{
				ImGui::SetKeyboardFocusHere();
			}

			if (ImGui::Button("Ok", ImVec2(80, 0)))
			{
				// convert the regBuffer buffer to a u16
				u16 value = (u16)strtol(regBuffer, NULL, 16);

				if (strlen(regBuffer) > 0)
				{
					switch(reg)
					{
						case AF: Cpu::af.reg = value; break;
						case BC: Cpu::bc.reg = value; break;
						case DE: Cpu::de.reg = value; break;
						case HL: Cpu::hl.reg = value; break;
						case SP: Cpu::sp.reg = value; break;
						case PC: Cpu::pc.reg = value; break;
						case LCDC: Memory::WriteByte(Memory::Address::LCDC, value & 0xFF);  break;
						case STAT: Memory::WriteByte(Memory::Address::STAT, value & 0xFF); break;
						case LY: Memory::WriteByte(Memory::Address::LY, value & 0xFF); break;
						case IME:  break;
						case IF: Memory::WriteByte(Memory::Address::IF, value & 0xFF); break;
						case IE: Memory::WriteByte(Memory::Address::IE, value & 0xFF); break;
						case TIMA: Memory::WriteByte(Memory::Address::TIMA, value & 0xFF); break;
						case TAC: Memory::WriteByte(Memory::Address::TAC, value & 0xFF); break;
						case TMA: Memory::WriteByte(Memory::Address::TMA, value & 0xFF); break;
						case DIV: Memory::WriteByte(Memory::Address::DIV, value & 0xFF); break;
						case Z: if ((value & 0xF) == 1) Flags::Set(Flags::z); else Flags::Clear(Flags::z); break;
						case N: if ((value & 0xF) == 1) Flags::Set(Flags::n); else Flags::Clear(Flags::n); break;
						case H: if ((value & 0xF) == 1) Flags::Set(Flags::h); else Flags::Clear(Flags::h); break;
						case C: if ((value & 0xF) == 1) Flags::Set(Flags::c); else Flags::Clear(Flags::c); break;
					}
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(80, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal(setBreakPtTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
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

				// increase the program counter if needed, so we can run to the same breakpoint again
				if (Cpu::pc.reg == breakpoint) Cpu::pc.reg += 1;
				ImGui::CloseCurrentPopup();
			}
			else
			{
				ImGui::OpenPopup(breakPtErrorTitle);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::BeginPopupModal(breakPtErrorTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::SetWindowSize(breakPtErrorTitle, ImVec2(230, 210));
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
	const char *stateLoadFailTitle = "State Load Failed";

	ImGui::Begin(title, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(25, 0, 51));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(102, 102, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0, 0, 204));

	if (ImGui::Button("Open Rom", ImVec2(width - 16, 0)))
	{
		char const *validExtensions[4] = {"*.gb", "*.GB", "*.bin", "*.BIN"};
		const char *filename = tinyfd_openFileDialog("Select Rom", "", 4, validExtensions, NULL, 0);

		if (filename != NULL)
		{
			stepThrough = true;
			stopAtBreakpoint = false;

			ResetSystem(filename);
		}
	}

	if (ImGui::Button("Save State", ImVec2(width - 16, 0)))
	{
		Cpu::SaveState();
	}

	if (ImGui::Button("Load State", ImVec2(width - 16, 0)))
	{
		if (!Cpu::LoadState())
		{
			ImGui::OpenPopup(stateLoadFailTitle);
		}
	}

	if (ImGui::Button("Hide Debugger", ImVec2(width - 16, 0)))
	{
		debuggerActive = false;
		// should scale the game to full screen here too...
	}

	if (ImGui::Button("Quit", ImVec2(width - 16, 0)))
	{
		exit(0);
	}

	ImGui::PopStyleColor(3);

	if (ImGui::BeginPopupModal(stateLoadFailTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::SetWindowSize(stateLoadFailTitle, ImVec2(230, 150));
		ImGui::TextWrapped("Failed to find save state in the executable directory");
		ImGui::NewLine();

		if (ImGui::Button("Ok", ImVec2(200, 0)))
		{
				ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
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
