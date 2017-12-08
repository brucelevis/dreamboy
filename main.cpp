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
#include "src/imgui/imgui.h"
#include "src/imgui/imgui_impl_sdl.h"
#include "src/imgui/imgui_custom_extensions.h"
#include "src/includes/debugger.h"
#include "src/includes/cpu.h"
#include "src/includes/rom.h"

// defines
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FRAMERATE 60
#define EMULATOR_NAME "DreamBoy - GameBoy Emulator"
// vars
static SDL_Window *window;
static SDL_GLContext glContext;
static bool quit = false;
static bool usingBios = false;

// responsible for initializing GL
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
		io.Fonts->AddFontFromFileTTF("Cousine-Regular.ttf", 16.0f);

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
		printf("InitSDL() - Error creating OGL Context\n");
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
			printf("InitSDL() - Error creating SDL Window\n");
			return false;
		}
	}

	return true;
}

// responsible for shutting down SDL
static void ShutdownSDL()
{
	ImGui_ImplSdlGL2_Shutdown();
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
}

// responsible for the emulation loop
static void EmulationLoop()
{
	Cpu::cycles = 0;

	while (Cpu::cycles < (MAX_CYCLES / FRAMERATE))
	{
		int cycleCount = Cpu::cycles;
		Cpu::ExecuteOpcode();
		int currentCycle = (Cpu::cycles - cycleCount);
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

						case SDLK_UP:
						case SDLK_DOWN:
							if (Debugger::stepThrough) EmulationLoop();
						break;

						default: break;
					}
				break;
			}
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//if (!Debugger::stepThrough) EmulationLoop();

		ImGui_ImplSdlGL2_NewFrame(window);
		// show the debugger windows
		Debugger::ControlsWindow("Controls", 156, 210, SCREEN_WIDTH - 456, 5);
		Debugger::RomInfoWindow("Rom Info", 240, 270, SCREEN_WIDTH - 480, SCREEN_HEIGHT - 260);
		Debugger::FileWindow("File", 156, 270, 0, SCREEN_HEIGHT - 260);
		Debugger::MemoryViewerWindow("Mem View", 100, 210, SCREEN_WIDTH - 289, 5, 0xC000, 0xFFFF);
		Debugger::RegisterViewer("Reg View", 180, 210, SCREEN_WIDTH - 180, 5);
		Debugger::MemoryEditorWindow("Program Flow", 240, 200, SCREEN_WIDTH - 234, SCREEN_HEIGHT - 260);
		ImGui::Render();

		SDL_GL_SwapWindow(window);
	}
}

int main(int argc, char *argv[])
{
	if (InitSDL())
	{
		Cpu::Init(usingBios);
		Rom::Load("roms/tests/cpu_instrs/07-jr,jp,call,ret,rst.gb");
		StartMainLoop();
	}

	ShutdownSDL();

	return 0;
}
