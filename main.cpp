// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Main loop and other stuff.

#ifdef _WIN32
#include <windows.h>
#include "SDL_syswm.h"
#endif
#include "maze.h"

TheGame* TheGame::m_instance = NULL;

int main(int argc, char* argv[])
{
	TheGame* game = new TheGame();

	MazeSettings settings;
	if(argc == 2) settings.difficulty = atoi(argv[1]);
	else if(argc == 5)
	{
		settings.difficulty = atoi(argv[1]);
		settings.fullscreen = atoi(argv[2]);
		settings.wnd_width = atoi(argv[3]);
		settings.wnd_height = atoi(argv[4]);
	}

	game->SetSettings(settings);
	int ret = game->Run();
	exit(ret);
}

void report_error(string err)
{
	#if defined _WIN32 && not defined ERR_CONSOLE_OUTPUT
	HWND wnd;
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if(!SDL_GetWMInfo(&wmi)) wnd = NULL;
	else wnd = wmi.window;
	MessageBoxA(wnd, err.c_str(), "Error", MB_OK | MB_ICONERROR);
	#else
	cerr << "Error: " << err << endl;
	#endif
}

int TheGame::Run()
{
	try
	{
		video.Init();
	}
	catch(MazeException e)
	{
		report_error(e.What());
		if(e.ShouldTerminate()) return 1;
	}
	try
	{
		sound.SoundInit();
	}
	catch(MazeException e)
	{
		report_error(e.What());
		if(e.ShouldTerminate()) return 1;
	}

	MainLoop();

	Cleanup();
	return 0;
}

void TheGame::MainLoop()
{
	const float FPS = 35.0;

	// Simple FPS limiter. We certainly should tie the movements
	// to the real time passing and never to the frames drawn.
	Uint32 last_time = SDL_GetTicks();
	Uint32 wait;
	frames_drawn = 0;

	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_GrabInput(SDL_GRAB_ON);

	do
	{
		try
		{
			ProcessEvents();
			video.Draw();
			sound.Do();
		}
		catch(MazeException e)
		{
			report_error(e.What());
			if(e.ShouldTerminate()) should_stop = true;
		}
		//cout << "xpos: " << player.xpos << "\tzpos: " << player.zpos << endl;
		frames_drawn++;

		wait = SDL_GetTicks() - last_time;
		if(wait < 1000.0 / FPS) SDL_Delay( (Uint32)(1000.0 / FPS - wait) );
		last_time = SDL_GetTicks();
	} while(!should_stop);

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(SDL_ENABLE);
}

void TheGame::Cleanup()
{
	sound.DieSound();
	SDL_Quit();
}
