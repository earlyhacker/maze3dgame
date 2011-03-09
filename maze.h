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
#include <SDL/SDL.h>
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
	}
	void MainLoop();
	int Run();
	void VideoInit();
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
	bool should_stop;
	short frames_drawn;
	MazeSettings settings;
	double yaw; // yaw is nose right, nose left
	double pitch; // pitch is nose up, nose down
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
