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
#include <SDL2/SDL_opengl.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_custom_extensions.h"
#include "includes/bios.h"
#include "includes/debugger.h"
#include "includes/cpu.h"
#include "includes/input.h"
#include "includes/interrupts.h"
#include "includes/lcd.h"
#include "includes/log.h"
#include "includes/memory.h"
#include "includes/rom.h"
#include "includes/timer.h"
#include "tinydir/tinydir.h"
#include "includes/typedefs.h"
#include "includes/ui.h"

// defines
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define EMULATOR_NAME "DreamBoy - GameBoy Emulator"
// vars
static SDL_Window *window;
static SDL_GLContext glContext;
static bool quit = false;
static bool ctrlPressed = false;
static int keyPressed = -1;
const char *cpuTests[12] =
{
	NULL,
	"roms/tests/cpu_instrs/01-special.gb", "roms/tests/cpu_instrs/02-interrupts.gb",
	"roms/tests/cpu_instrs/03-op sp,hl.gb", "roms/tests/cpu_instrs/04-op r,imm.gb",
	"roms/tests/cpu_instrs/05-op rp.gb", "roms/tests/cpu_instrs/06-ld r,r.gb",
	"roms/tests/cpu_instrs/07-jr,jp,call,ret,rst.gb", "roms/tests/cpu_instrs/08-misc instrs.gb",
	"roms/tests/cpu_instrs/09-op r,r.gb", "roms/tests/cpu_instrs/10-bit ops.gb",
	"roms/tests/cpu_instrs/11-op a,(hl).gb",
};

// responsible for initializing OpenGL
static bool InitGL()
{
	glContext = SDL_GL_CreateContext(window);

	if (glContext)
	{
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		ImGui_ImplSdlGL2_Init(window);
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("Cousine-Bold.ttf", 16.0f);

		ImGuiStyle &style = ImGui::GetStyle();
		style.AntiAliasedLines = false;
		style.AntiAliasedShapes = false;
		style.FrameRounding = 0.0f;
		style.WindowRounding = 0.0f;
		style.ScrollbarRounding = 0.0f;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glShadeModel(GL_FLAT);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DITHER);
		glDisable(GL_BLEND);
	}
	else
	{
		Log::Critical("InitSDL() - Error creating OGL Context");
		return false;
	}

	return true;
}

// responsible for initializing SDL
static bool InitSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) >= 0)
	{
		window  = SDL_CreateWindow(EMULATOR_NAME,  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

		if (window)
		{
			if (!InitGL()) return false;
		}
		else
		{
			Log::Critical("InitSDL() - Error creating SDL Window");
			return false;
		}
	}

	return true;
}

// responsible for creating initial directories
static void CreateDirectories()
{
	struct stat st = {0};

	if (stat("screenshots", &st) == -1) mkdir("screenshots", 0700);
	if (stat("saves", &st) == -1) mkdir("saves", 0700);
	if (stat("saves/states", &st) == -1) mkdir("saves/states", 0700);
	if (stat("saves/states/debugger", &st) == -1) mkdir("saves/states/debugger", 0700);
}

// responsible for shutting down SDL + misc stuff
static void Shutdown()
{
	Log::Close();
	ImGui_ImplSdlGL2_Shutdown();
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
}

// responsible for executing operations before the app quits
static void OnAppQuit()
{
	Rom::SaveRam();
	Debugger::RemoveStates();
	quit = true;
}

// responsible for displaying the various debugger windows
static void ShowDebugger()
{
	if (!Debugger::active) return;

	// show the debugger windows
	Debugger::RegisterViewerWindow("Reg View", 180, 250, SCREEN_WIDTH - 180, 22);
	Debugger::MemoryViewerWindow("Program Flow", 180, 230, SCREEN_WIDTH - 180, SCREEN_HEIGHT - 210);
}

// responsible for the emulation loop
static void EmulationLoop()
{
	Cpu::cycles = 0;

	while (Cpu::cycles < (MAX_CYCLES / Cpu::framerate))
	{
		if (Debugger::stopAtBreakpoint && (Cpu::pc.reg == Debugger::breakpoint))
		{
			Debugger::stepThrough = true;
			break;
		}

		Cpu::Step();
	}
}

// responsible for the main loop
static void StartMainLoop()
{
	SDL_Event event;

	while (!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			Input::HandleKeys(event);
			ImGui_ImplSdlGL2_ProcessEvent(&event);

			switch(event.type)
			{
				case SDL_QUIT:
					OnAppQuit();
				break;

				case SDL_MOUSEMOTION:
				{
					int mouseX;
					int mouseY;
					SDL_GetMouseState(&mouseX, &mouseY);

					if (mouseY <= 24) Ui::ShowMainMenuBar();
				}
				break;

				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_LCTRL: ctrlPressed = true; break;
						default: keyPressed = event.key.keysym.sym; break;
					}
				break;

				case SDL_KEYUP:
					switch(event.key.keysym.sym)
					{
						case SDLK_LCTRL: ctrlPressed = false; break;
						default: keyPressed = -1;
					}
				break;
			}
		}

		// handle key combos for ctrl
		if (ctrlPressed)
		{
			switch(keyPressed)
			{
				// quit
				case SDLK_q: ctrlPressed = false; OnAppQuit(); break;
				// hide the menu bar
				case SDLK_m: Ui::HideMainMenuBar(); break;
				// save a state
				case SDLK_s: ctrlPressed = false; Cpu::SaveState(false); break;
				// load a state
				case SDLK_l: ctrlPressed = false; Cpu::LoadState(false); break;
				//  save a screenshot
				case SDLK_p: ctrlPressed = false; Debugger::SaveScreenshot(); break;
				// enable/disable the debugger
				case SDLK_d: ctrlPressed = false; Debugger::active = !Debugger::active; break;
				// run/stop
				case SDLK_r:
					ctrlPressed = false;
					Debugger::stepThrough = false;
					Debugger::stopAtBreakpoint = false;
					Ui::HideMainMenuBar();
				break;
				// open the select rom popup
				case SDLK_o: ctrlPressed = false; Debugger::SelectRom(); break;
				// close the rom
				case SDLK_c: ctrlPressed = false; Debugger::ResetSystem(); break;
				// step forward
				case SDLK_f:
					ctrlPressed = false;

					if (Debugger::stepThrough)
					{
						Cpu::Step();
						Cpu::SaveState(true, Cpu::instructionsRan);
					}
				break;
				// step backward
				case SDLK_b:
					ctrlPressed = false;

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
				break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplSdlGL2_NewFrame(window);

		if (!Debugger::stepThrough) EmulationLoop();

		Lcd::Render();
		ShowDebugger();
		Ui::Render();
		ImGui::Render();

		SDL_GL_SwapWindow(window);
	}
}

int main(int argc, char *argv[])
{
	if (InitSDL())
	{
		CreateDirectories();
		Log::Init();
		Memory::Init();
		//Rom::Load(cpuTests[2]);

		//Cpu::didLoadBios = Bios::Load("bios.bin");
		Interrupts::Init();
		Cpu::Init();
		Timer::Init();
		Lcd::Init();
		Input::Init();
		StartMainLoop();
	}

	Shutdown();

	return 0;
}
