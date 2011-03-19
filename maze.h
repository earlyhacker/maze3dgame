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
//#include "SDL_ttf.h"
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

class TheSound
{
	public:

	TheSound()
	{
		audio_rate = 44100;
		audio_format = MIX_DEFAULT_FORMAT;
		audio_channels = 2;
		audio_buffers = 4096;
		backMusic = NULL;
		stepSound = NULL;
	}

	void SoundInit();
	void PlaySound(const char* track, int loop);
	void DieSound();

	//private:

	int audio_rate;
	int audio_channels;
	int audio_buffers;

	Mix_Chunk* backMusic;
	Mix_Chunk* stepSound;
	Uint16 audio_format; /* 16-bit stereo */
};


// Does the main work like rendering, input processing, etc
// Do not create instances of this class, use TheGame::Get()
class TheGame
{
	public:

	TheGame()
	{
		if(m_instance == NULL) m_instance = this;
		should_stop = false;
		yaw = 0.0;
		pitch = 0.0;
	}
	static void MusicFinished();
	void MainLoop();
	int Run();

	void VideoInit();
	void TTFInit();
	void Cleanup();

	void ProcessEvents();
	void Draw();
	void CreateLists();
	static TheGame* Get()
	{
		return m_instance;
	}
	void SetSettings(MazeSettings _settings)
	{
		settings = _settings;
	}
	MazeSettings GetSettings()
	{
		return settings;
	}
	void ReportError(string);

	TheSound sound;


	private:
	static TheGame* m_instance;
	bool should_stop;
	short frames_drawn;
	MazeSettings settings;
	double yaw; // yaw is nose right, nose left
	double pitch; // pitch is nose up, nose down

	// Display lists. This all doesn't look too good.
	static const short LIST_COUNT = 9;
	enum {
		LIST_WALL=0,
		LIST_CORNER,
		LIST_WALL_BRANCH,
		LIST_CORNER_BRANCH,
		LIST_STRAIGHT_PASS,
		LIST_RIGHT_TURN,
		LIST_LEFT_TURN,
		LIST_DEAD_END,
		LIST_RIGHT_BRANCH,
		LIST_LEFT_BRANCH
	};
	GLuint display_lists[LIST_COUNT];
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
