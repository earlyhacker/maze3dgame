// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Contains rendering and video initialization code.

#include "maze.h"

void TheGame::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glFlush();
	SDL_GL_SwapBuffers();

	frames_drawn++;
}

void TheGame::VideoInit()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER| SDL_INIT_AUDIO) < 0)
		throw MazeException(string("SDL initialization failed")+SDL_GetError());
	atexit(SDL_Quit);

	unsigned int flags = SDL_OPENGL | SDL_DOUBLEBUF;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// I don't know what is the big deal with those attributes and whether they
	// suit our needs. I'll research this later.|| don't try =)
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	if(SDL_SetVideoMode(settings.wnd_width, settings.wnd_height, 32, flags) == NULL)
		throw MazeException(string("Could not set video mode")+SDL_GetError());

	glViewport(0, 0, settings.wnd_width, settings.wnd_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, settings.wnd_width / settings.wnd_height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	SDL_WM_SetCaption("Maze 3d version undefined","ex1");
	//SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void TheGame::SoundInit()
{
	// Although ogg and mp3 functionality is nice, so far we're relying
	// on WAV only.
	int flags = MIX_INIT_OGG | MIX_INIT_MOD | MIX_INIT_MP3;
	Mix_Init(flags);

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
		throw MazeException(string("Unable to open audio device: ") + Mix_GetError());
}
