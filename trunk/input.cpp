// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Dealing with input devices.

#include "maze.h"

//static Mix_Music* TheGame::music;

/*void TheGame::MusicFinished() {
  Mix_HaltMusic();
  Mix_FreeChunk(music);
  music = NULL;
}*/

void TheGame::ProcessEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			should_stop = true;
			Mix_CloseAudio();
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
				if(backMusic == NULL) {
					backMusic = Mix_LoadWAV("music.wav");
					if(backMusic == NULL) printf("loading: %s\n", Mix_GetError());
					if(Mix_PlayChannel(1, backMusic, 1)==-1)// play on 1 channel.
						printf("Mix_PlayChannel: %s\n",Mix_GetError());
						//Mix_PlayMusic(music, 0);
					//Mix_HookMusicFinished(&TheGame::MusicFinished);
				} else {
					//Mix_HaltMusic();
					Mix_FreeChunk(backMusic);
					backMusic = NULL;
				}
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

