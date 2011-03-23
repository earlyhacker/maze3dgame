// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Main loop and other stuff.

#ifdef _WIN32
#include <windows.h>
#endif
#include "maze.h"

TheGame* TheGame::m_instance = NULL;

int main(int argc, char* argv[])
{
	TheGame* game = new TheGame();
	MazeSettings settings;
	game->SetSettings(settings);
	int ret = game->Run();
	exit(ret);
}

void TheGame::ReportError(string err)
{
	#ifdef _WIN32
	MessageBoxA(NULL, err.c_str(), "Exception caught", 0);
	#else
	cerr << "Exception caught: " << err << endl;
	#endif
}

int TheGame::Run()
{
	try
	{
		VideoInit();
		sound.SoundInit();
	}
	catch(MazeException e)
	{
		ReportError(e.What());
		return 1;
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
		ProcessEvents();
		Draw();

		wait = SDL_GetTicks() - last_time;
		if(wait < 1000.0 / FPS) SDL_Delay( (Uint32)(1000.0 / FPS - wait) );
		last_time = SDL_GetTicks();
	} while(!should_stop);

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(SDL_ENABLE);
}

void TheGame::Cleanup()
{
	SDL_Quit();
	// TODO: add whatever they have in OpenAL for cleaning things up
}
