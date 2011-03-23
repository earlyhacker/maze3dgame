// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Dealing with input devices.

#include "maze.h"




void TheGame::ProcessEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			should_stop = true;
//			Mix_CloseAudio();
			SDL_Quit();
			break;
		case SDL_MOUSEMOTION:
			yaw += event.motion.xrel;
			pitch += event.motion.yrel;
			break;
		case SDL_KEYDOWN:
			SDLMod mod = SDL_GetModState();
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				should_stop = true;
				break;
			case SDLK_m:
				if(sound.buffer ==NULL)
				sound.PlaySound("Data/music.wav", 1);
				else sound.DieSound();
				break;
			case SDLK_F4:
				// For unknown reasons SDL applications won't respond to
				// pressing ALT+F4, something few people would like.
				if(mod & KMOD_ALT) should_stop = true;
				break;
			case SDLK_r:
				yaw = 0;
				pitch = 0;
				break;
			}
			break;
		}
	}

}

