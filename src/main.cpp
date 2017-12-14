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
#include "includes/debugger.h"
#include "includes/cpu.h"
#include "includes/interrupts.h"
#include "includes/lcd.h"
#include "includes/log.h"
#include "includes/memory.h"
#include "includes/rom.h"
#include "includes/timer.h"
#include "includes/typedefs.h"

// defines
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FRAMERATE 60
#define EMULATOR_NAME "DreamBoy - GameBoy Emulator"
// vars
static SDL_Window *window;
static SDL_GLContext glContext;
static bool quit = false;
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
	if (SDL_Init(SDL_INIT_VIDEO) >= 0)
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

// responsible for shutting down SDL + misc stuff
static void Shutdown()
{
	Log::Close();
	ImGui_ImplSdlGL2_Shutdown();
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
}

// responsible for displaying the various debugger windows
static void ShowDebugger()
{
	// show the debugger windows
	Debugger::ControlsWindow("Controls", 156, 250, SCREEN_WIDTH - 456, 5);
	Debugger::MemoryViewerWindow("Memory", 100, 250, SCREEN_WIDTH - 289, 5, 0xC000, 0xFFFF);
	Debugger::RegisterViewer("Reg View", 180, 250, SCREEN_WIDTH - 180, 5);
	Debugger::FileWindow("File", 156, 250, 0, SCREEN_HEIGHT - 215);
	Debugger::RomInfoWindow("Rom Info", 240, 250, SCREEN_WIDTH - 480, SCREEN_HEIGHT - 215);
	Debugger::MemoryEditorWindow("Program Flow", 240, 230, SCREEN_WIDTH - 234, SCREEN_HEIGHT - 215);
}

// responsible for stepping the cpu
static void CpuStep()
{
	int cycleCount = Cpu::cycles;

	Cpu::ExecuteOpcode();
	Timer::Update(Cpu::cycles - cycleCount);
	Lcd::Update(Cpu::cycles - cycleCount);
	Interrupts::Service();
}

// responsible for the emulation loop
static void EmulationLoop()
{
	Cpu::cycles = 0;

	while (Cpu::cycles < (MAX_CYCLES / FRAMERATE))
	{
		if (Debugger::stopAtBreakpoint && (Cpu::pc.reg == Debugger::breakpoint))
		{
			Debugger::stepThrough = true;
			break;
		}

		CpuStep();
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
			ImGui_ImplSdlGL2_ProcessEvent(&event);

			switch(event.type)
			{
				case SDL_QUIT: quit = true;

				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE: quit = true; break;

						case SDLK_0: Debugger::stepThrough = true; break;

						case SDLK_UP:
							if (Debugger::stepThrough)
							{
								if (Cpu::instructionsRan > 0)
								{
									Cpu::LoadState(Cpu::instructionsRan);
									Cpu::instructionsRan -= 1;
								}
								else
								{
									Debugger::ResetSystem();
								}
							}
						break;

						case SDLK_DOWN:
							if (Debugger::stepThrough)
							{
								CpuStep();
								Cpu::SaveState(Cpu::instructionsRan);
							}
						break;

						default: break;
					}
				break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplSdlGL2_NewFrame(window);

		if (!Debugger::stepThrough) EmulationLoop();

		ShowDebugger();
		ImGui::Render();
		Lcd::Render();

		SDL_GL_SwapWindow(window);
	}
}

int main(int argc, char *argv[])
{
	if (InitSDL())
	{
		Log::Init();
		Memory::Init();

		Cpu::didLoadBios = false;
		Rom::Load(cpuTests[2]);
		Cpu::Init();
		Timer::Init();
		Lcd::Init();
		Interrupts::Init();
		StartMainLoop();
	}

	Shutdown();

	return 0;
}
