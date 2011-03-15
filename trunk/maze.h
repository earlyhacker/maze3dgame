// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// The main header file for the project.

#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#ifdef _MSC_VER
#include <sdl_opengl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <SDL.h>
#include "SDL_mixer.h"
#include "SDL_ttf.h"
using namespace std;

// Contains application settings
class MazeSettings
{
	public:
	MazeSettings(); // Loads defaults
	void Read();
	void Store();

	int wnd_width, wnd_height;
};

// Does the main work like rendering, input processing, etc
class TheGame
{
	public:

	TheGame()
	{
		initialized = false;
		should_stop = false;
		yaw = 0.0;
		pitch = 0.0;
		audio_rate = 44100;
		audio_format = AUDIO_S16;
		audio_channels = 2;
		audio_buffers = 4096;
		music = Mix_LoadWAV("music.wav");;
		screen = SDL_SetVideoMode(settings.wnd_width, settings.wnd_height, 0, SDL_HWSURFACE | SDL_DOUBLEBUF); 
		 // first it's name of font, second it's size of font;
		
	}
	static void MusicFinished();
	void MainLoop();
	int Run();
	void VideoInit();
	void TTFInit();
	void SoundInit();
	void ProcessEvents();
	void SetSettings(MazeSettings _settings)
	{
		settings = _settings;
	}
	MazeSettings GetSettings()
	{
		return settings;
	}
	void Draw();
	void ReportError(string);
	private:

	bool initialized;
	static SDL_Surface* screen; 
	bool should_stop;
	short frames_drawn;
	MazeSettings settings;
	double yaw; // yaw is nose right, nose left
	double pitch; // pitch is nose up, nose down
	int audio_rate;
	Mix_Chunk *music;
	Uint16 audio_format; /* 16-bit stereo */
	int audio_channels;
	int audio_buffers;
	
	
};

class MazeException
{
	public:
	MazeException(const string err)
	{
		m_err = err;
	}
	string What()
	{
		return m_err;
	}
	private:
	string m_err;
};

#endif // MAZE_H

//SDL_Surface* screen = SDL_SetVideoMode(settings.wnd_width, settings.wnd_height, 0, SDL_HWSURFACE | SDL_DOUBLEBUF); 
/*void print_ttf(SDL_Surface *sDest, char* message, char* font, int size, 
		SDL_Color color, SDL_Rect dest)
	{
		TTF_Font *fnt = TTF_OpenFont(font, size);
		SDL_Surface *sText = TTF_RenderText_Blended( fnt, message, color);
		SDL_BlitSurface( sText,NULL, sDest,&dest );
		SDL_FreeSurface( sText );
		TTF_CloseFont( fnt );
	}*/