/*
 * DreamBoy - A Nintendo GameBoy Emulator
 * Written in C/C++
 * Author: Daniel Glover: http://github.com/dannyglover/
 * License:  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 * Copyright 2017 - Danny Glover. All rights reserved.
 */

#define STB_IMAGE_WRITE_IMPLEMENTATION

// includes
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_custom_extensions.h"
#include "imgui/imgui_memory_editor.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "includes/bios.h"
#include "includes/debugger.h"
#include "includes/flags.h"
#include "includes/input.h"
#include "includes/interrupts.h"
#include "includes/lcd.h"
#include "includes/log.h"
#include "includes/memory.h"
#include "includes/rom.h"
#include "stb/stb_image_write.h"
#include "tinydir/tinydir.h"
#include "includes/timer.h"
#include "includes/ui.h"

// init vars
bool Debugger::stepThrough = false;
bool Debugger::stopAtBreakpoint = false;
bool Debugger::active = false;
bool Debugger::stopMachine = false;
u16 Debugger::breakpoint = 0x00;
const char *Debugger::modifyRegistersPopupTitle = "Modify Registers/Flags";
const char *Debugger::memViewPopupTitle = "Memory View";
const char *Debugger::memGetValuePopupTitle = "Get Memory Value";
const char *Debugger::memSetValuePopupTitle = "Set Memory Value";
const char *Debugger::setBreakpointPopupTitle = "Set Breakpoint";
const char *Debugger::romInfoPopupTitle = "Rom Information";
static char breakpointBuffer[256];
static char regBuffer[256];
static char memBuffer[256];
static char memValueBuffer[256];
static char memSetBuffer[256];
static char memSetAddressBuffer[256];
static MemoryEditor memoryViewer;
struct Reg
{
	enum name
	{
		AF, BC, DE, HL, SP, PC, LCDC, STAT, LY, IME, IF, IE, TIMA, TAC, TMA, DIV, Z, N, H, C
	};
};
static int modRegister = Reg::name::AF;

// responsible for resetting the system
void Debugger::ResetSystem(bool reloadRom)
{
	stopMachine = false;
	Memory::Init();
	if (reloadRom) Rom::Reload();
	if (Cpu::didLoadBios) Bios::Reload();
	Cpu::Init();
	Input::Init();
	Timer::Init();
	Lcd::Init();
	Interrupts::Init();
}

// responsible for saving a screenshot
void Debugger::SaveScreenshot()
{
	char outputFilename[512];
	char outputFilePath[512];
	const char *filename;
	(filename = strrchr(Rom::filename, '/')) ? ++filename : (filename = Rom::filename);

	// get the current file extension and rename it to jpg
	sscanf(filename,"%[^.]",outputFilename);
	sprintf(outputFilename,"%s.jpg",outputFilename);
	sprintf(outputFilePath,"screenshots/%s",outputFilename);
	stbi_write_jpg(outputFilePath, 160, 144, 3, Lcd::screen, 100);
}

// responsible for showing the debugger
void Debugger::ShowDebugger()
{
	active = true;
	// make the GameBoy Lcd occupy the entire screen
	Lcd::width = 160;
	Lcd::height = 144;
}

// responsible for hiding the debugger
void Debugger::HideDebugger()
{
	active = false;
	// make the GameBoy Lcd occupy the entire screen
	Lcd::width = 640;
	Lcd::height = 480;
}

// responsible for displaying the view memory popup
void Debugger::ViewMemoryPopup()
{
	if (ImGui::BeginPopupModal(memViewPopupTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		int i = 0xFFFF;

		ImGui::SetWindowSize(memViewPopupTitle, ImVec2(208, 380));

		if (ImGui::Button("Close", ImVec2(192, 0))) ImGui::CloseCurrentPopup();

		ImGui::BeginChild("mem");

		while(i  > 0x0000)
		{
			ImGuiExtensions::TextWithColors("{FF0000}%04X: {FFFFFF}%02X\t{FF0000}%04X: {FFFFFF}%02X", i, Memory::ReadByte(i), i - 1, Memory::ReadByte(i - 1));
			i -= 2;
		}

		ImGui::EndChild();
		ImGui::EndPopup();
	}
}

// responsible for displaying the get memory value popup
void Debugger::GetMemoryValuePopup()
{
	if (ImGui::BeginPopupModal(memGetValuePopupTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::SetWindowSize(memGetValuePopupTitle, ImVec2(190, 180));
		ImGui::PushItemWidth(175);
		ImGui::Text("Enter Address:");

		if (ImGui::InputText("a", memBuffer, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll))
		{
			u16 address = (u16)strtol(memBuffer, NULL, 16);
			sprintf(memValueBuffer, "%02X", Memory::ReadByte(address));
		}

		ImGui::Text("Value At Address:");
		ImGui::InputText("b", memValueBuffer, 5, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
		ImGui::NewLine();

		if (ImGui::Button("Close", ImVec2(175, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

// responsible for displaying the set memory value popup
void Debugger::SetMemoryValuePopup()
{
	if (ImGui::BeginPopupModal(memSetValuePopupTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::SetWindowSize(memSetValuePopupTitle, ImVec2(190, 180));
		ImGui::PushItemWidth(180);
		ImGui::Text("Enter Address:");
		ImGui::InputText("a", memSetAddressBuffer, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::Text("Enter Value:");
		ImGui::InputText("b", memSetBuffer, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::NewLine();

		if (ImGui::Button("Submit", ImVec2(80, 0)))
		{
			u16 address = (u16)strtol(memSetAddressBuffer, NULL, 16);
			u8 data = (u8)strtol(memSetBuffer, NULL, 16);

			Memory::WriteByte(address, data);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Close", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

// responsible for removing previous save states
void Debugger::RemoveStates()
{
	struct stat st = {0};

	if (stat("saves/states/debugger", &st) >= 0)
	{
		char filePath[512];
		tinydir_dir dir;
		tinydir_open(&dir, "saves/states/debugger");

		while (dir.has_next)
		{
			tinydir_file file;
			tinydir_readfile(&dir, &file);
			sprintf(filePath, "saves/states/debugger/%s", file.name);

			if (!file.is_dir) remove(filePath);

			tinydir_next(&dir);
		}

		tinydir_close(&dir);
	}
}

// responsible for dumping memory to a file
void Debugger::DumpMemory()
{
	char const *validExtensions[4] = {"*.bin", "*.BIN", "*.dump", "*.dump"};
	const char *filename = tinyfd_saveFileDialog("Save As", "", 4, validExtensions, NULL);

	if (filename != NULL)
	{
		FILE *fp = fopen(filename, "wb");
		fwrite(Memory::mem, sizeof(Memory::mem), 1, fp);
	}
}

// responsible for importing memory from a file
void Debugger::ImportMemory()
{
	char const *validExtensions[4] = {"*.bin", "*.BIN", "*.dump", "*.dump"};
	const char *filename = tinyfd_openFileDialog("Select Memory Dump", "", 4, validExtensions, NULL, 0);

	if (filename != NULL)
	{
		FILE *fp = fopen(filename, "rb");
		fread(&Memory::mem, 1, 0x10000, fp);
	}
}

// responsible for displaying the rom info popup
void Debugger::RomInfoPopup()
{
	if (ImGui::BeginPopupModal(romInfoPopupTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::SetWindowSize(romInfoPopupTitle, ImVec2(400, 380));
		ImGuiExtensions::TextWithColors("{FF0000}Name:"); ImGui::SameLine();

		// display the rom name
		for (u16 i = Memory::Address::ROM_NAME_START; i < Memory::Address::ROM_NAME_END; i++)
		{
			if (Memory::ReadByte(i) != 0)
			{
				ImGui::Text("%c", Memory::ReadByte(i));
				ImGui::SameLine();
			}
		}

		ImGui::NewLine();
		ImGuiExtensions::TextWithColors("{FF0000}Type: {FFFFFF}%02X", Memory::ReadByte(Memory::Address::ROM_TYPE));
		ImGuiExtensions::TextWithColors("{FF0000}Rom-Size: {FFFFFF}%02X", Memory::ReadByte(Memory::Address::ROM_SIZE));
		ImGuiExtensions::TextWithColors("{FF0000}Ram-Size: {FFFFFF}%02X", Memory::ReadByte(Memory::Address::ROM_RAM_SIZE));
		ImGuiExtensions::TextWithColors("{FF0000}File Path:");
		ImGui::SameLine();
		ImGui::TextWrapped("%s", Rom::filename);
		ImGui::NewLine();

		if (ImGui::Button("Close", ImVec2(385, 0))) ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

// responsible for displaying the set breakpoint popup
void Debugger::SetBreakpointPopup()
{
	const char *breakPtErrorPopup = "Error!";

	if (ImGui::BeginPopupModal(setBreakpointPopupTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
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
				ImGui::OpenPopup(breakPtErrorPopup);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(80, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		// breakpoint error popup
		if (ImGui::BeginPopupModal(breakPtErrorPopup, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::SetWindowSize(breakPtErrorPopup, ImVec2(230, 210));
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
}

// responsible for displaying the modify registers popup
void Debugger::ModifyRegistersPopup()
{
	const char *setRegPopup = "Set Value";

	if (ImGui::BeginPopupModal(modifyRegistersPopupTitle, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::SetWindowSize(modifyRegistersPopupTitle, ImVec2(280, 310));
		ImGui::Text("Choose The Register To Modify:");
		ImGui::NewLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(25, 0, 51));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(102, 102, 255));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0, 0, 204));

		// block 1
		if (ImGui::Button("AF", ImVec2(60, 0)))
		{
			modRegister = Reg::name::AF;
			sprintf(regBuffer, "%04X", Cpu::af.reg);
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("BC", ImVec2(60, 0)))
		{
			modRegister = Reg::name::BC;
			sprintf(regBuffer, "%04X", Cpu::bc.reg);
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("DE", ImVec2(60, 0)))
		{
			modRegister = Reg::name::DE;
			sprintf(regBuffer, "%04X", Cpu::de.reg);
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("HL", ImVec2(60, 0)))
		{
			modRegister = Reg::name::HL;
			sprintf(regBuffer, "%04X", Cpu::hl.reg);
			ImGui::OpenPopup(setRegPopup);
		}

		// block 2
		if (ImGui::Button("SP", ImVec2(60, 0)))
		{
			modRegister = Reg::name::SP;
			sprintf(regBuffer, "%04X", Cpu::sp.reg);
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("PC", ImVec2(60, 0)))
		{
			modRegister = Reg::name::PC;
			sprintf(regBuffer, "%04X", Cpu::pc.reg);
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("LCDC", ImVec2(60, 0)))
		{
			modRegister = Reg::name::LCDC;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::LCDC));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("STAT", ImVec2(60, 0)))
		{
			modRegister = Reg::name::STAT;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::STAT));
			ImGui::OpenPopup(setRegPopup);
		}

		// block 3
		if (ImGui::Button("LY", ImVec2(60, 0)))
		{
			modRegister = Reg::name::LY;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::LY));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("IME", ImVec2(60, 0)))
		{
			modRegister = Reg::name::IME;
			sprintf(regBuffer, "%d", Interrupts::ime);
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("IF", ImVec2(60, 0)))
		{
			modRegister = Reg::name::IF;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::IF));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("IE", ImVec2(60, 0)))
		{
			modRegister = Reg::name::IE;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::IE));
			ImGui::OpenPopup(setRegPopup);
		}

		// block 4
		if (ImGui::Button("TIMA", ImVec2(60, 0)))
		{
			modRegister = Reg::name::TIMA;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::TIMA));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("TAC", ImVec2(60, 0)))
		{
			modRegister = Reg::name::TAC;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::TAC));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("TMA", ImVec2(60, 0)))
		{
			modRegister = Reg::name::TMA;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::TMA));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("DIV", ImVec2(60, 0)))
		{
			modRegister = Reg::name::DIV;
			sprintf(regBuffer, "%02X", Memory::ReadByte(Memory::Address::DIV));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::NewLine();
		ImGui::Text("Choose A Flag To Modify:");
		ImGui::NewLine();

		// block 5
		if (ImGui::Button("Z", ImVec2(60, 0)))
		{
			modRegister = Reg::name::Z;
			sprintf(regBuffer, "%d", Flags::Get(Flags::z));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("N", ImVec2(60, 0)))
		{
			modRegister = Reg::name::N;
			sprintf(regBuffer, "%d", Flags::Get(Flags::n));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("H", ImVec2(60, 0)))
		{
			modRegister = Reg::name::H;
			sprintf(regBuffer, "%d", Flags::Get(Flags::h));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::SameLine();

		if (ImGui::Button("C", ImVec2(60, 0)))
		{
			modRegister = Reg::name::C;
			sprintf(regBuffer, "%d", Flags::Get(Flags::c));
			ImGui::OpenPopup(setRegPopup);
		}

		ImGui::PopStyleColor(3);
		ImGui::NewLine();
		ImGui::Indent(60.f);

		if (ImGui::Button("Close", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		// set register popup
		if (ImGui::BeginPopupModal(setRegPopup, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
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
					switch(modRegister)
					{
						case Reg::name::AF: Cpu::af.reg = value; break;
						case Reg::name::BC: Cpu::bc.reg = value; break;
						case Reg::name::DE: Cpu::de.reg = value; break;
						case Reg::name::HL: Cpu::hl.reg = value; break;
						case Reg::name::SP: Cpu::sp.reg = value; break;
						case Reg::name::PC: Cpu::pc.reg = value; break;
						case Reg::name::LCDC: Memory::WriteByte(Memory::Address::LCDC, value & 0xFF);  break;
						case Reg::name::STAT: Memory::WriteByte(Memory::Address::STAT, value & 0xFF); break;
						case Reg::name::LY: Memory::mem[Memory::Address::LY] = (value & 0xFF); break;
						case Reg::name::IME: Interrupts::ime = (value & 0x1); break;
						case Reg::name::IF: Memory::WriteByte(Memory::Address::IF, value & 0xFF); break;
						case Reg::name::IE: Memory::WriteByte(Memory::Address::IE, value & 0xFF); break;
						case Reg::name::TIMA: Memory::WriteByte(Memory::Address::TIMA, value & 0xFF); break;
						case Reg::name::TAC: Memory::WriteByte(Memory::Address::TAC, value & 0xFF); break;
						case Reg::name::TMA: Memory::WriteByte(Memory::Address::TMA, value & 0xFF); break;
						case Reg::name::DIV: Memory::WriteByte(Memory::Address::DIV, value & 0xFF); break;
						case Reg::name::Z: if ((value & 0xF) == 1) Flags::Set(Flags::z); else Flags::Clear(Flags::z); break;
						case Reg::name::N: if ((value & 0xF) == 1) Flags::Set(Flags::n); else Flags::Clear(Flags::n); break;
						case Reg::name::H: if ((value & 0xF) == 1) Flags::Set(Flags::h); else Flags::Clear(Flags::h); break;
						case Reg::name::C: if ((value & 0xF) == 1) Flags::Set(Flags::c); else Flags::Clear(Flags::c); break;
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
}

// create a memory viewer window
void Debugger::MemoryViewerWindow(const char *title, int width, int height, int x, int y)
{
	ImGui::Begin(title);
	ImGui::SetWindowSize(title, ImVec2(width, height));
	ImGui::SetWindowPos(title, ImVec2(x, y));
	memoryViewer.DrawContents(Memory::mem, 0x10000, 0x0000);
	memoryViewer.GotoAddrAndHighlight(Cpu::pc.reg, Cpu::pc.reg);
	ImGui::End();
}

// responsible for displaying the register viewer window
void Debugger::RegisterViewerWindow(const char *title, int width, int height, int x, int y)
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
	ImGuiExtensions::TextWithColors("{FF0000}TIMA: {FFFFFF}%02X", Memory::ReadByte(Memory::Address::TIMA)); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}DIV:  {FFFFFF}%02X", Memory::ReadByte(Memory::Address::DIV)); ImGui::Unindent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}TAC:  {FFFFFF}%02X", Memory::ReadByte(Memory::Address::TAC)); ImGui::SameLine(); ImGui::Indent(80.f);
	ImGuiExtensions::TextWithColors("{FF0000}TMA:  {FFFFFF}%02X", Memory::ReadByte(Memory::Address::TMA)); ImGui::SameLine(); ImGui::Unindent(80.f);
	ImGui::NewLine();
	ImGuiExtensions::TextWithColors("{FF0000}Ins Ran: {FFFFFF}%d", Cpu::instructionsRan);
	ImGui::Checkbox("Z", &flagZ); ImGui::SameLine();
	ImGui::Checkbox("N", &flagN); ImGui::SameLine();
	ImGui::Checkbox("H", &flagH); ImGui::SameLine();
	ImGui::Checkbox("C", &flagC); ImGui::SameLine();
	ImGui::End();
}
