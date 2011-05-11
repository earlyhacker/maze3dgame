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
	SoundList();
}

void TheSound::SoundList()
{	
	// c 1 - 4 звуки игрока, остальные звуки обстановки.
	OpenSound("Data/sound/steps.wav",   1, 1, 0.5,    1.0f, -5.0f, -0.2f);
	OpenSound("Data/sound/torch.wav",	2, 0, 1.0,	  0.0f, 0.0f, 0.0f);// for torch selector
	

	OpenSound("Data/sound/bgmusic.wav", 5, 1, 0.1,    1.0f, -5.0f, -100.0f);
	OpenSound("Data/sound/water.wav",   6, 1, 1.0,    1.0f, -1.0f, 0.0f);

	OpenSound("Data/sound/hum.wav",	    7, 0, 0.5,    1.0f, -3.0f, 0.0f);// файлы играющие рандомно
	OpenSound("Data/sound/flash.wav",   8, 0, 0.6,    1.0f, -1.0f, 0.0f);//
	
	PlaySound(5);
	PlaySound(6);

}

void TheSound::RandPlay()// рандомно включает музыку, наверно его надо воткнуть в мэйн луп, что бы работало.
{ // так как не могу проверить то вот описание: 
	// берется число от СДЛ_тика (он в милисек.) и число 3600(это минута в милисек) и прибавляется сдл_тик,
	// так мы получили бесконечный счетчик. Так же рандомно берется и трек. Всего два - гул и замыкание.
	float rand_time = 0.0f;
	Uint32 timer = SDL_GetTicks();//ms -> 1 minutes = 3600 ms
	int max_time = 3600 + timer;
	float track =0;
	track = (rand()% (2 - 1 + 1)) + 1;

	rand_time = (rand()% (max_time - timer + 1)) + timer; //(rand() % (max - min + 1)) + min
	
	if (rand_time == SDL_GetTicks)
		if(track == 1)
			PlaySound(7);
		else PlaySound(8);

}

void TheSound::OpenSound(const char* track, int index, int loop, float volume, float x, float y, float z)
{
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
