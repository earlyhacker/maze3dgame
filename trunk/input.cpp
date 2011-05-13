// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Dealing with input devices.

#include "maze.h"


void TheGame::ProcessEvents()
{
	ThePlayer& player = TheGame::Get()->player;
	const MazeSettings& cfg = TheGame::Get()->GetSettings();
	float yaw_rad = player.yaw * M_PI/180; // yaw in radians
	float pitch_rad = player.pitch * M_PI/180;
	const float mouse_factor = 0.32;
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			should_stop = true;
			break;
		case SDL_MOUSEMOTION:
			player.yaw += event.motion.xrel * mouse_factor;
			player.pitch += event.motion.yrel * mouse_factor;
			if(player.pitch > 70) player.pitch = 70;
			if(player.pitch < -70) player.pitch = -70;
			break;
		case SDL_MOUSEBUTTONUP:
			if(event.button.button == SDL_BUTTON_LEFT)
			{
				if(player.light_on)
				{
					sound.PlaySound(2);
					glDisable(GL_LIGHT0);
					player.light_on = false;
				}
				else
				{
					sound.PlaySound(2);
					glEnable(GL_LIGHT0);
					player.light_on = true;
				}
			}
			break;
		case SDL_KEYDOWN:
			SDLMod mod = SDL_GetModState();
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				should_stop = true;
				break;
			case SDLK_m:
				//sound.PlaySound();
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
				glClearColor(0.0, 0.0, 0.0, 1.0);
				video.start = maze_build(cfg.difficulty, cfg.difficulty);
				player.current_section = video.start;
				player.xpos = 0;
				player.ypos = 2.5;
				player.zpos = -1.5;
				break;
			case SDLK_0:
				player.ChangeLight(0);
				break;
			case SDLK_1:
				player.ChangeLight(1);
				break;
			case SDLK_2:
				player.ChangeLight(2);
				break;
			case SDLK_3:
				player.ChangeLight(3);
				break;
			case SDLK_4:
				player.ChangeLight(4);
				break;
			}
			break;
		}
	}

	Uint8* kb_state = SDL_GetKeyState(NULL);
	const float speed = 0.133;
	if(kb_state[cfg.keys[MOVE_FORTH]] || kb_state[cfg.keys[MOVE_BACK]] ||
			kb_state[cfg.keys[MOVE_RIGHT]] || kb_state[cfg.keys[MOVE_LEFT]])
	{
		if(!player.walking)
		{
			player.walking = true;
			sound.PlaySound(1);
		}
	}
	else
	{
		if(player.walking)
		{
			player.walking = false;
			sound.StopSound(1);
		}
	}
	if(kb_state[cfg.keys[MOVE_FORTH]])
	{
		player.Move(-sin(-yaw_rad) * cos(pitch_rad) * speed,
				-sin(pitch_rad) * speed,
				-cos(-yaw_rad) * cos(pitch_rad) * speed);
	}
	if(kb_state[cfg.keys[MOVE_BACK]])
	{
		player.Move(sin(-yaw_rad) * cos(pitch_rad) * speed,
				sin(pitch_rad) * speed,
				cos(-yaw_rad) * cos(pitch_rad) * speed);
	}
	if(kb_state[cfg.keys[MOVE_RIGHT]])
	{
		player.Move(-sin(-yaw_rad - M_PI/2) * speed, 0,
				-cos(-yaw_rad - M_PI/2) * speed);
	}
	if(kb_state[cfg.keys[MOVE_LEFT]])
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
	// So now that the y doesn't change, he's kind of standing on the ground.
	//float y = ypos + dy;
	float y = ypos;
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
		zpos = -zpos;
		xpos = xpos * cos(-sec->rot * M_PI/180) - zpos * sin(-sec->rot * M_PI/180);
		zpos = xpos * sin(-sec->rot * M_PI/180) + zpos * cos(-sec->rot * M_PI/180);
		zpos = -zpos;
		zpos = 0; // Screw it!! At least this works.
		// FIXME
		if(zpos > 0)
		{
			cerr << "FIXME FIXME FIXME FIXME!!! zpos = " << zpos << endl;
			zpos = 0;
		}
		yaw += sec->rot;
		current_section = sec;
		return true;
	case 0x12:
		// FIXME: looks no better than the above
		sec = current_section->links[2];
		zpos = -zpos;
		xpos = xpos * cos(current_section->rot * M_PI/180) -
			zpos * sin(current_section->rot * M_PI/180);
		zpos = xpos * sin(current_section->rot * M_PI/180) +
			zpos * cos(current_section->rot * M_PI/180);
		zpos = -zpos;
		xpos += current_section->trans.x;
		zpos += current_section->trans.z;
		yaw -= current_section->rot;
		current_section = sec;
		return true;
	default:
		if(key < 0x30)
			throw MazeException(string("ThePlayer::Move(): unknown key"));
	}
	if(key >= 0x30) // the if is reduntant, but just in case...
	{
		if(current_section->Trigger(key))
		{
			xpos = x;
			ypos = y;
			zpos = z;
			return true;
		}
		else return false;
	}
}

void ThePlayer::Walk(float x, float z)
{
	// to be done
}

// TODO: We're of course better off using enum instead of plain integer, and
// also placing this function in ThePlayer is somewhat questionable.
void ThePlayer::ChangeLight(int n)
{
	GLfloat l_none[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat l_full[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat l_yellowish[] = { 1.0, 0.94, 0.86, 1.0 };
	switch(n)
	{
	case 0:
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l_none);
		break;
	case 1:
		// http://tinyurl.com/3hzjopo
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.29);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.075);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l_yellowish);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
		break;
	case 2:
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.12);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.011);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l_full);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 34);
		break;
	case 3:
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0015);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l_full);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 63);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 25); // 17
		break;
	case 4:
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l_full);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 25);
		break;
	default:
		throw MazeException("Unknown lighting mode index");
		break;
	}
}

FinalSection::FinalSection(const TubeSection& sec) : TubeSection(sec)
{
	if(sec.type != DEAD_END)
	{
		cerr << "sec.type == " << sec.type << endl;
		throw MazeException("FinalSection type must be DEAD_END!");
	}
	paintover.push_back(PaintOverRect(0x42, -2.5, -10, 5, 5));
}

bool FinalSection::Trigger(char c)
{
	TheGame::Get()->sound.StopSound(1);
	TheGame::Get()->sound.StopSound(5);
	TheGame::Get()->sound.StopSound(7);
	TheGame::Get()->sound.PlaySound(3);
	TheGame::Get()->player.ypos = 400;
	TheGame::Get()->player.ChangeLight(4);
	cout << "Exit found\n";
	glClearColor(0.7, 0.8, 0.9, 1.0);
	return false;
}
