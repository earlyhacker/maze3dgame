// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Contains rendering and video initialization code.

#include "maze.h"


void TheVideo::Draw()
{
	ThePlayer& player = TheGame::Get()->player;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glColor3f(0.7, 0.5, 0.7);
	// This requires more detalization and I want it to be lightweight.
	/*GLfloat specular[] = { 0.75, 0.75, 0.75, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 21);*/

	GLfloat light_pos[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	glRotatef(player.yaw, 0, cos(player.pitch * M_PI/180),
			sin(player.pitch * M_PI/180));
	glRotatef(player.pitch, 1, 0, 0);
	glTranslatef(-player.xpos, -player.ypos, -player.zpos);

	//There is a great deal of difference between an eager man who wants to read a book and a tired man who wants a book to read. (c) Chesterton, Essays

	glCallList(player.current_section->list);
	if(player.current_section->links[0])
		RunThrough(player.current_section->links[0]);
	if(player.current_section->links[1])
		RunThrough(player.current_section->links[1]);
	if(player.current_section->links[3])
		RunThrough(player.current_section->links[3]);
	if(player.current_section->links[2])
		RunThrough(player.current_section, true);

	glFlush();
	SDL_GL_SwapBuffers();
}

// FIXME: It's not working right
void TheVideo::RunThrough(TubeSection* section, bool backwards)
{
	TubeSection* sec = section;
	glPushMatrix();
	if(!backwards)
	{
		glTranslatef(sec->trans.x, 0, sec->trans.z);
		glRotatef(sec->rot, 0, 1, 0);
		glCallList(sec->list);
	}
	else
	{
		glRotatef(-sec->rot, 0, 1, 0);
		glTranslatef(-sec->trans.x, 0, -sec->trans.z);
		glCallList(sec->links[2]->list);
		sec = sec->links[2];
	}
	while(1)
	{
		if(sec->links[1])
		{
			glPushMatrix();
			glTranslatef(sec->links[1]->trans.x, 0, sec->links[1]->trans.z);
			glRotatef(sec->links[1]->rot, 0, 1, 0);
			glCallList(sec->links[1]->list);
			glPopMatrix();
		}
		if(sec->links[3])
		{
			glPushMatrix();
			glTranslatef(sec->links[3]->trans.x, 0, sec->links[3]->trans.z);
			glRotatef(sec->links[3]->rot, 0, 1, 0);
			glCallList(sec->links[3]->list);
			glPopMatrix();
		}
		if(!backwards)
		{
			if(sec->links[0])
			{
				glTranslatef(sec->links[0]->trans.x, 0, sec->links[0]->trans.z);
				glRotatef(sec->links[0]->rot, 0, 1, 0);
				glCallList(sec->links[0]->list);
				sec = sec->links[0];
			}
			else break;
		}
		else
		{
			if(sec->links[2])
			{
				glRotatef(-sec->rot, 0, 1, 0);
				glTranslatef(-sec->trans.x, 0, -sec->trans.z);
				glCallList(sec->links[2]->list);
				sec = sec->links[2];
			}
			else break;
		}
	}
	glPopMatrix();
}

void TheVideo::CreateLists()
{
	// For now everything is hard-coded, model loading will come if need be.

	GLuint start_index = glGenLists(6);
	for(int i = 0; i < LIST_COUNT; i++)
		dlists[i] = start_index + i;

	float d, d2; // d stands for delta

	// A wall
	glNewList(dlists[LIST_WALL], GL_COMPILE);
	glBegin(GL_QUADS);
	for(float i = 0; i < 10 - 0.001; i += 0.2)
		for(float j = 0; j < 5 - 2*crn_off - 0.001; j += 0.2)
		{
			glVertex3f(0, 2.5 - crn_off - j, - i);
			glVertex3f(0, 2.5 - crn_off - j - 0.2, -i);
			glVertex3f(0, 2.5 - crn_off - j - 0.2, -i - 0.2);
			glVertex3f(0, 2.5 - crn_off - j, -i - 0.2);
		}
	glEnd();
	glEndList();

	// A corner cut
	glNewList(dlists[LIST_CORNER], GL_COMPILE);
	glBegin(GL_QUADS);
	for(float i = 0; i < 10 - 0.001; i += 0.2)
	{
		glVertex3f(crn_off, crn_off, -i);
		glVertex3f(0, 0, -i);
		glVertex3f(0, 0, -i - 0.2);
		glVertex3f(crn_off, crn_off, -i - 0.2);
	}
	glEnd();
	glEndList();

	// It's a plank. Nothing more than just a plank.
	glNewList(dlists[LIST_BRANCH_PLANK], GL_COMPILE);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glNormal3f(0, 0, 1);
	d = (5 - 2*crn_off) / 10;
	d2 = (trn_off + crn_off) / 4;
	glBegin(GL_QUADS);
	for(float i = 0; i < 5 - 2*crn_off - 0.001; i += d)
		for(float j = 0; j < trn_off + crn_off - 0.001; j += d2)
		{
			glVertex3f(j, i, 0);
			glVertex3f(j + d2, i, 0);
			glVertex3f(j + d2, i + d, 0);
			glVertex3f(j, i + d, 0);
		}
	glEnd();
	glPopMatrix();
	glEndList();

	// DUDE! The current normal is transformed by the modelview matrix
	// at the point of specifying a vertex. No. Freakin. Earlier.

	// Straight pass-thru
	glNewList(dlists[LIST_STRAIGHT_PASS], GL_COMPILE);

	glNormal3f(1, 0, 0);
	glPushMatrix();
	glTranslatef(-2.5, 2.5, 0);
	glCallList(dlists[LIST_WALL]);
	glNormal3f(-1, 0, 0);
	glTranslatef(5, 0, 0);
	glCallList(dlists[LIST_WALL]);
	glPopMatrix();
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glNormal3f(1, 0, 0);
	glCallList(dlists[LIST_WALL]);
	glTranslatef(5, 0, 0);
	glNormal3f(-1, 0, 0);
	glCallList(dlists[LIST_WALL]);
	glPopMatrix();

	glNormal3f(0.7071, -0.7071, 0);
	glPushMatrix();
	glTranslatef(-2.5, 5 - crn_off, 0);
	glCallList(dlists[LIST_CORNER]);
	glTranslatef(5, 0, 0);
	glScalef(-1, 1, 1);
	glCallList(dlists[LIST_CORNER]);
	glPopMatrix();
	glPushMatrix();
	glNormal3f(-0.7071, 0.7071, 0);
	glTranslatef(2.5 - crn_off, 0, 0);
	glCallList(dlists[LIST_CORNER]);
	glTranslatef(-5 + 2*crn_off, 0, 0);
	glScalef(-1, 1, 1);
	glCallList(dlists[LIST_CORNER]);
	glPopMatrix();

	glEndList();

	// A dead-end
	glNewList(dlists[LIST_DEAD_END], GL_COMPILE);
	glCallList(dlists[LIST_STRAIGHT_PASS]);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	// crazy hack for the lazy
	for(float i = 0; i < 5 - 0.001; i += 0.2)
		for(float j = 0; j < 5 - 0.001; j += 0.2)
		{
			glVertex3f(-2.5 + j, 5 - i, -10);
			glVertex3f(-2.5 + j, 5 - i - 0.2, -10);
			glVertex3f(-2.5 + j + 0.2, 5 - i - 0.2, -10);
			glVertex3f(-2.5 + j + 0.2, 5 - i, -10);
		}
	/*glVertex3f(-2.5 + crn_off, 5, -10);
	glVertex3f(-2.5, 5 - crn_off, -10);
	glVertex3f(-2.5, crn_off, -10);
	glVertex3f(-2.5 + crn_off, 0, -10);
	glVertex3f(2.5 - crn_off, 0, -10);
	glVertex3f(2.5, crn_off, -10);
	glVertex3f(2.5, 5 - crn_off, -10);
	glVertex3f(2.5 - crn_off, 5, -10);*/
	glEnd();
	glEndList();

	// A block used to build two-way passages
	glNewList(dlists[LIST_WALL_BRANCH], GL_COMPILE);
	glNormal3f(1, 0, 0);
	glBegin(GL_QUADS);
	for(float i = 0; i < 2.5 - trn_off; i += (2.5 - trn_off) / 14)
		for(float j = 0; j < 5 - 2*crn_off; j += 0.2)
		{
			glVertex3f(0, 2.5 -crn_off - j, -i);
			glVertex3f(0, 2.5 - crn_off - j - 0.2, -i);
			glVertex3f(0, 2.5 - crn_off - j - 0.2, -i - (2.5 - trn_off) / 14);
			glVertex3f(0, 2.5 - crn_off - j, -i - (2.5 - trn_off) / 14);
		}
	glEnd();
	glPushMatrix();
	glTranslatef(0, 0, -2.5 + trn_off);
	glBegin(GL_QUADS);
	glNormal3f(0.7071, 0, -0.7071);
	d = trn_off / 5;
	for(float i = 0; i < trn_off; i += d)
		for(float j = 0; j < 5 - 2*crn_off; j += 0.2)
		{
			glVertex3f(-i, 2.5 - crn_off - j, -i);
			glVertex3f(-i, 2.5 - crn_off - j - 0.2, -i);
			glVertex3f(-i - d, 2.5 - crn_off - j - 0.2, -i - d);
			glVertex3f(-i - d, 2.5 - crn_off - j, -i - d);
		}
	d = (crn_off + trn_off) / 5;
	for(float i = 0; i < crn_off + trn_off; i += d)
	{
		glNormal3f(0, 1, 0);
		glVertex3f(crn_off - i, -2.5, -i);
		glVertex3f(crn_off, -2.5, -i);
		glVertex3f(crn_off, -2.5, -i - d);
		glVertex3f(crn_off - i -d, -2.5, -i - d);

		glNormal3f(0, -1, 0);
		glVertex3f(crn_off - i, 2.5, -i);
		glVertex3f(crn_off, 2.5, -i);
		glVertex3f(crn_off, 2.5, -i - d);
		glVertex3f(crn_off - i -d, 2.5, -i - d);
	}
	glEnd();

	// Way to go, the light will be broken on those
	glBegin(GL_QUADS);
	glNormal3f(cos(-M_PI/4)*cos(M_PI/4), sin(-M_PI/4), -cos(-M_PI/4)*sin(M_PI/4));
	glVertex3f(crn_off, 2.5, 0);
	glVertex3f(0, 2.5 - crn_off, 0);
	glVertex3f(-trn_off, 2.5 - crn_off, -trn_off);
	glVertex3f(-trn_off, 2.5, -trn_off - crn_off);

	glNormal3f(cos(M_PI/4)*cos(M_PI/4), sin(M_PI/4), -cos(M_PI/4)*sin(M_PI/4));
	glVertex3f(crn_off, -2.5, 0);
	glVertex3f(0, -2.5 + crn_off, 0);
	glVertex3f(-trn_off, -2.5 + crn_off, -trn_off);
	glVertex3f(-trn_off, -2.5, -trn_off - crn_off);
	glEnd();
	glPopMatrix();

	glBegin(GL_QUADS);
	for(float i = 0; i < 2.5 - trn_off; i += (2.5 - trn_off) / 14)
	{
		glNormal3f(0.7071, -0.7071, 0);
		glVertex3f(crn_off, 2.5, -i);
		glVertex3f(0, 2.5 - crn_off, -i);
		glVertex3f(0, 2.5 - crn_off, -i - (2.5 - trn_off) / 14);
		glVertex3f(crn_off, 2.5, -i - (2.5 - trn_off) / 14);

		glNormal3f(0.7071, 0.7071, 0);
		glVertex3f(0, -2.5 + crn_off, -i);
		glVertex3f(crn_off, -2.5, -i);
		glVertex3f(crn_off, -2.5, -i - (2.5 - trn_off) / 14);
		glVertex3f(0, -2.5 + crn_off, -i - (2.5 - trn_off) / 14);
	}
	glEnd();
	glEndList();

	// A passage with a left turn
	glNewList(dlists[LIST_LEFT_BRANCH], GL_COMPILE);
	glPushMatrix();
	glTranslatef(-2.5, 2.5, 0);
	glCallList(dlists[LIST_WALL_BRANCH]);
	glPushMatrix();
	glTranslatef(0, 0, -10);
	glScalef(1, 1, -1);
	glCallList(dlists[LIST_WALL_BRANCH]);
	glPopMatrix();
	glTranslatef(5, 0, 0);
	glNormal3f(-1, 0, 0);
	glCallList(dlists[LIST_WALL]);
	glPopMatrix();
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glNormal3f(1, 0, 0);
	glCallList(dlists[LIST_WALL]);
	glTranslatef(5, 0, 0);
	glNormal3f(-1, 0, 0);
	glCallList(dlists[LIST_WALL]);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(2.5, 5 - crn_off, 0);
	glScalef(-1, 1, 1);
	glNormal3f(0.7071, -0.7071, 0);
	glCallList(dlists[LIST_CORNER]);
	glTranslatef(0, -5 + 2*crn_off, 0);
	glScalef(1, -1, 1);
	glCallList(dlists[LIST_CORNER]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5 - trn_off, 0, -2.5 - crn_off);
	glCallList(dlists[LIST_BRANCH_PLANK]);
	glTranslatef(0, 5, 0);
	glScalef(1, -1, 1);
	glCallList(dlists[LIST_BRANCH_PLANK]);
	glPopMatrix();

	glEndList();

	glNewList(dlists[LIST_RIGHT_BRANCH], GL_COMPILE);
	glPushMatrix();
	glScalef(-1, 1, 1);
	glCallList(dlists[LIST_LEFT_BRANCH]);
	glPopMatrix();
	glEndList();

	// Left turn
	glNewList(dlists[LIST_LEFT_TURN], GL_COMPILE);
	glPushMatrix();
	glTranslatef(-2.5, 2.5, 0);
	glCallList(dlists[LIST_WALL_BRANCH]);
	glTranslatef(5, 0, 0);
	glNormal3f(-1, 0, 0);
	glScalef(1, 1, 0.75);
	glCallList(dlists[LIST_WALL]);
	glTranslatef(-2.5, 2.5, 0);
	glRotatef(90, 0, 0, 1);
	glNormal3f(-1, 0, 0);
	glCallList(dlists[LIST_WALL]);
	glTranslatef(-5, 0, 0);
	glNormal3f(1, 0, 0);
	glCallList(dlists[LIST_WALL]);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(2.5, 5 - crn_off, 0);
	glScalef(-1, 1, 0.75);
	glNormal3f(0.7071, -0.7071, 0);
	glCallList(dlists[LIST_CORNER]);
	glTranslatef(0, -5 + 2*crn_off, 0);
	glScalef(1, -1, 1);
	glCallList(dlists[LIST_CORNER]);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2.5 + crn_off, 0, -7.5);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	for(float i = 0; i < 5 - crn_off - 0.001; i += 0.2)
		for(float j = 0; j < 5 - 0.001; j += 0.2)
		{
			glVertex3f(i, j, 0);
			glVertex3f(i, j + 0.2, 0);
			glVertex3f(i + 0.2, j + 0.2, 0);
			glVertex3f(i + 0.2, j, 0);
		}
	glEnd();

	glTranslatef(-crn_off - trn_off, crn_off, 0);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glCallList(dlists[LIST_BRANCH_PLANK]);
	glPopMatrix();
	glTranslatef(0, -crn_off, 5 - crn_off);
	glCallList(dlists[LIST_BRANCH_PLANK]);
	glTranslatef(0, 5, 0);
	glScalef(1, -1, 1);
	glCallList(dlists[LIST_BRANCH_PLANK]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5 + crn_off, 2.5, -7.5);
	glBegin(GL_TRIANGLES);
	glNormal3f(1, 0, 0);
	glVertex3f(0, -2.5, 0);
	glVertex3f(0, -2.5 + crn_off, 0);
	glVertex3f(0, -2.5, crn_off);
	glVertex3f(0, 2.5, 0);
	glVertex3f(0, 2.5 - crn_off, 0);
	glVertex3f(0, 2.5, crn_off);
	glEnd();
	glBegin(GL_QUADS);
	d = (crn_off + trn_off) / 5;
	for(float i = 0; i < trn_off + crn_off - 0.001; i += d)
	{
		glNormal3f(0, 0.7071, 0.7071);
		glVertex3f(-i, -2.5 + crn_off, 0);
		glVertex3f(-i, -2.5, crn_off);
		glVertex3f(-i - d, -2.5, crn_off);
		glVertex3f(-i - d, -2.5 + crn_off, 0);

		glNormal3f(0, -0.7071, 0.7071);
		glVertex3f(-i, 2.5 - crn_off, 0);
		glVertex3f(-i, 2.5, crn_off);
		glVertex3f(-i - d, 2.5, crn_off);
		glVertex3f(-i - d, 2.5 - crn_off, 0);
	}
	glEnd();

	glTranslatef(-crn_off - trn_off, -2.5, 2.5);
	glRotatef(90, 0, 1, 0);
	glScalef(1, 1, (2.5 - trn_off) / 10);
	glCallList(dlists[LIST_STRAIGHT_PASS]);

	glPopMatrix();
	glEndList();


	glNewList(dlists[LIST_RIGHT_TURN], GL_COMPILE);
	glPushMatrix();
	glScalef(-1, 1, 1);
	glCallList(dlists[LIST_LEFT_TURN]);
	glPopMatrix();
	glEndList();

	glNewList(dlists[LIST_FORK], GL_COMPILE);
	glPushMatrix();
	glTranslatef(5, 0, -2.5 - trn_off);
	glRotatef(90, 0, 1, 0);
	glCallList(dlists[LIST_LEFT_BRANCH]);
	glPopMatrix();
	glEndList();
}

void TheVideo::Init()
{
	MazeSettings cfg = TheGame::Get()->GetSettings();

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER| SDL_INIT_AUDIO) < 0)
		throw MazeException(string("SDL initialization failed")+SDL_GetError());
	atexit(SDL_Quit);

	unsigned int flags = SDL_OPENGL | SDL_DOUBLEBUF;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// I don't know what is the big deal with those attributes and whether they
	// suit our needs. I'll research this later.|| don't try =)
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	if(SDL_SetVideoMode(cfg.wnd_width, cfg.wnd_height, 32, flags) == NULL)
		throw MazeException(string("Could not set video mode")+SDL_GetError());

	glViewport(0, 0, cfg.wnd_width, cfg.wnd_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, cfg.wnd_width / cfg.wnd_height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	SDL_WM_SetCaption("Maze 3d version undefined","ex1");
	//SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat no_ambient[] = { 0.05, 0.05, 0.05, 1 }; // well, almost
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, no_ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.25); // 0.07

	CreateLists();
	start = maze_build(7, 7);
	ThePlayer& player = TheGame::Get()->player;
	player.ypos = 2.5;
	player.zpos = -1.5;
	player.current_section = start;
}

void TheVideo::TexInit()
{
	ilInit();
	iluInit();
	GetErrIL();

}
void TheVideo::LoadTex(const char *FileName)
{
	ilLoad(IL_JPG, reinterpret_cast<const ILstring>(FileName));
	GetErrIL();
	ImgWidth = ilGetInteger(IL_IMAGE_WIDTH);
	ImgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	Imgbpp = ilGetInteger(IL_IMAGE_BPP);
	unsigned char* data = ilGetData();
	unsigned int type;
// переопределить тип для OpenGL
	switch (Imgbpp) 
	{
	case 1:
		type  = GL_RGB8;
	break;
	case 3:
		type = GL_RGB;
	break;
	case 4:
		type = GL_RGBA;
	break;
	}
	unsigned int IndexTexture = -1;
	glGenTextures(1, &IndexTexture);  
	glBindTexture(GL_TEXTURE_2D, IndexTexture); 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);	
	gluBuild2DMipmaps(GL_TEXTURE_2D, Imgbpp, ImgWidth, ImgHeight, type, 
                      GL_UNSIGNED_BYTE, data);
}

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
