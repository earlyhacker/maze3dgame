// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Sound-related stuff.

#include "maze.h"


void TheSound::SoundInit()
{

	if (!alutInit (NULL, NULL))
		throw MazeException(GetErrAL(), true);
	alListenerf(AL_GAIN, 2);
	SoundList();
}

void TheSound::SoundList()
{
	// c 1 - 4 звуки игрока, остальные звуки обстановки.
	OpenSound("Data/sound/footsteps.wav",   1, 1, 1.0,    0.0f, 0.0f, 0.0f);
	// the torch switch
	OpenSound("Data/sound/flashlight.wav",	2, 0, 0.6,	  0.0f, 0.0f, 0.0f);

	OpenSound("Data/sound/win.wav", 3, 0, 0.1, 0.0, 0.0, 0.0);


	OpenSound("Data/sound/bgmusic.wav", 5, 1, 0.08,    0.0f, 0.0f, 0.0f);
	OpenSound("Data/sound/water.wav",   6, 0, 0.62,    1.0f, 0.0f, 0.0f);

	OpenSound("Data/sound/hum.wav",	    7, 1, 0.5,    1.0f, 0.0f, 0.0f);
	OpenSound("Data/sound/flash.wav",   8, 0, 0.48,    1.0f, 0.0f, 0.0f);

	PlaySound(5);
	PlaySound(7);
}

void TheSound::Do()
{
	Uint32 cur_time = SDL_GetTicks();
	if(cur_time >= next_time)
	{
		// (rand() % (max - min + 1)) + min
		int track = rand() % 2;
		if(track) PlaySound(6);
		else PlaySound(8);
		next_time = cur_time + (rand() % 50001) + 40000;
	}
}

// Loads a file using the given index. loop is boolean 1|0. Setting gain > 1 is
// meaningless, as it gets clamped at 1.0 anyway. Changing the y coordinate
// doesn't make much sense either.
void TheSound::OpenSound(const char* track, int index, int loop, float volume, float x, float y, float z)
{
	// TODO: a temporary measure, I want a map<string, ALuint> here, like
	// in TheVideo::GetTexture()
	if(index >= 10) throw MazeException("I wanted a segfault and all I got was this lousy exception\nTheSound::OpenSound()");

	buffer = alutCreateBufferFromFile (track);

	if (buffer == AL_NONE)
    {
		throw MazeException(string("Error loading file ") + track + "  " +
				GetErrAL());
    }

	ALfloat mPos[3] = {x, y, z};// more fun
	alGenSources (1, &source[index]);
	alSourcei (source[index], AL_BUFFER, buffer);
	alSourcef (source[index], AL_PITCH,    1.0f);
	alSourcef (source[index], AL_GAIN,    volume);
	alSourcefv(source[index], AL_POSITION, mPos);
	//alSourcefv(source, AL_VELOCITY,  1.0f);
	alSourcei(source[index], AL_LOOPING, loop);
	GetErrAL();
}

bool TheSound::PlaySound(int index)
{
	alSourcePlay (source[index]);
	return true;
}

bool TheSound::StopSound(int index)
{
	alSourceStop(source[index]);
	return true;
}

void TheSound::DieSound()
{
	for(int i =0; i < 10; ++i)
	{
		alSourceStop(source[i]);
		if (alIsSource(source[i])) alDeleteSources(1, &source[i]);
	}
}
