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
	OpenSound("Data/file.au", 0);
}


void TheSound::OpenSound(const char* track, int loop)
{
	//Allum_Ind++;
	buffer = alutCreateBufferFromFile (track);
	if (buffer == AL_NONE)
    {
		throw MazeException(string("Error loading file ") + track + "  " +
				GetErrAL());
    }

	ALfloat mPos[3] = {1.0f, 1.0f, -5.0f};// more fun
	alGenSources (1, &source[Allum_Ind]);
	alSourcei (source[Allum_Ind], AL_BUFFER, buffer);
	alSourcef (source[Allum_Ind], AL_PITCH,    1.0f);
	alSourcef (source[Allum_Ind], AL_GAIN,    1.0f);
	alSourcefv(source[Allum_Ind], AL_POSITION, mPos);
	//alSourcefv(source, AL_VELOCITY,  1.0f);
	alSourcei(source[Allum_Ind], AL_LOOPING, loop);
	GetErrAL();
}

void TheSound::PlaySound()
{
	alSourcePlay (source[Allum_Ind]);
}

void TheSound::StopSound()
{
	alSourceStop(source[Allum_Ind]);
}

void TheSound::DieSound()
{
	for(int i =0; i <= Allum_Ind; i++)
	{
		alSourceStop(source[i]);
		if (alIsSource(source[i])) alDeleteSources(1, &source[i]);
	}
}
