// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Dealing with input devices.

#include "maze.h"


void TheGame::ProcessEvents()
{
	ThePlayer& player = TheGame::Get()->player;
	float yaw_rad = player.yaw * M_PI/180; // yaw in radians
	float pitch_rad = player.pitch * M_PI/180;
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			should_stop = true;
			break;
		case SDL_MOUSEMOTION:
			player.yaw += event.motion.xrel;
			player.pitch += event.motion.yrel;
			if(player.pitch > 90) player.pitch = 90;
			if(player.pitch < -90) player.pitch = -90;
			break;
		case SDL_KEYDOWN:
			SDLMod mod = SDL_GetModState();
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				should_stop = true;
				break;
			case SDLK_m:
				sound.PlaySound();
				break;
			case SDLK_f:
				if(glIsEnabled(GL_LIGHTING))
				{
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				else
				{
					glEnable(GL_LIGHTING);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				break;
			case SDLK_F4:
				// For unknown reasons SDL applications won't respond to
				// pressing ALT+F4, something few people would like.
				if(mod & KMOD_ALT) should_stop = true;
				break;
			case SDLK_r:
				player.yaw = 0;
				player.pitch = 0;
				break;
			case SDLK_n:
				video.start = maze_build(7, 7);
				player.current_section = video.start;
				player.xpos = 0;
				player.ypos = 2.5;
				player.zpos = -1.5;
				break;
			}
			break;
		}
	}

	Uint8* kb_state = SDL_GetKeyState(NULL);
	const float speed = 0.25;
	if(kb_state[SDLK_w])
	{
		player.Move(-sin(-yaw_rad) * cos(pitch_rad) * speed,
				-sin(pitch_rad) * speed,
				-cos(-yaw_rad) * cos(pitch_rad) * speed);
	}
	if(kb_state[SDLK_s])
	{
		player.Move(sin(-yaw_rad) * cos(pitch_rad) * speed,
				sin(pitch_rad) * speed,
				cos(-yaw_rad) * cos(pitch_rad) * speed);
	}
	if(kb_state[SDLK_d])
	{
		player.Move(-sin(-yaw_rad - M_PI/2) * speed, 0,
				-cos(-yaw_rad - M_PI/2) * speed);
	}
	if(kb_state[SDLK_a])
	{
		player.Move(sin(-yaw_rad - M_PI/2) * speed, 0,
				cos(-yaw_rad - M_PI/2) * speed);
	}
}

static bool is_in(float x, float z, const PaintOverRect& rect)
{
	return ( (x >= rect.x) && (x <= rect.x + rect.w) &&
			(z >= rect.z) && (z <= rect.z + rect.h) );
}

bool ThePlayer::Move(float dx, float dy, float dz)
{
	float x = xpos + dx;
	float y = ypos + dy;
	float z = zpos + dz;
	char key = 0x0;
	TubeSection* sec; // here we go. here as in place.

	if(y > 4.8 || y < 0.2) return false;

	vector<PaintOverRect>& paintover = current_section->paintover;
	for(int i = paintover.size() - 1; i >= 0; i--)
	{
		if(is_in(x, z, paintover[i]))
		{
			key = paintover[i].c;
			break;
		}
	}

	switch(key)
	{
	case 0x0:
		return false;
	case 0x1:
		xpos = x;
		ypos = y;
		zpos = z;
		return true;
	case 0x10:
	case 0x11:
	case 0x13:
		sec = current_section->links[key-0x10];
		xpos -= sec->trans.x;
		zpos -= sec->trans.z;
		xpos = xpos * cos(-sec->rot * M_PI/180) -
			-zpos * sin(-sec->rot * M_PI/180);
		zpos = xpos * sin(-sec->rot * M_PI/180) +
			-zpos * cos(-sec->rot * M_PI/180);
		zpos = -zpos;
		// FIXME
		if(zpos > 0)
		{
			cerr << "FIXME FIXME FIXME FIXME!!!\n";
			zpos = 0;
		}
		yaw += sec->rot;
		current_section = sec;
		return true;
	case 0x12:
		sec = current_section->links[2];
		xpos = xpos * cos(current_section->rot * M_PI/180) -
			-zpos * sin(current_section->rot * M_PI/180);
		zpos = xpos * sin(current_section->rot * M_PI/180) +
			-zpos * cos(current_section->rot * M_PI/180);
		zpos = -zpos;
		xpos += current_section->trans.x;
		zpos += current_section->trans.z;
		yaw -= current_section->rot;
		current_section = sec;
		return true;
	default:
		throw MazeException(string("ThePlayer::Move(): unknown key: ") + key);
	}
}
