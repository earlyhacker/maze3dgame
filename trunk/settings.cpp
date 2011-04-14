// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Implementation of the MazeSettings class, loading/storing settings in a file.

#include "maze.h"

MazeSettings::MazeSettings()
{
	wnd_width = 800;
	wnd_height = 600;

	keys[MOVE_FORTH] = SDLK_w;
	keys[MOVE_BACK] = SDLK_s;
	keys[MOVE_LEFT] = SDLK_a;
	keys[MOVE_RIGHT] = SDLK_d;
}

void MazeSettings::Read()
{
	int linecount;
	freopen("settings.txt", "r", stdin);
	cin >> linecount;
	for(int i=0; i <linecount; i++)
	{
		//cin >> MazeSettings::wnd_width;
	}
}


void MazeSettings::Store()
{
	// not implemented yet
}
