// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Contains rendering and video initialization code.

#include "maze.h"

void TheGame::Draw()
{
	static GLUquadric* qdr = gluNewQuadric();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glTranslatef(0, -2.5, -7);
	GLfloat light0_pos[] = { yaw/50, -pitch/50, -6.3, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glPushMatrix();
	glTranslatef(yaw/50, -pitch/50, -6.3);
	GLfloat yellow[] = { 0.8, 0.8, 0.0, 0.5 };
	GLfloat nuthin[] = { 0, 0, 0, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, yellow);
	gluSphere(qdr, 0.13, 50, 50);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, nuthin);
	glPopMatrix();

	glColor3f(0.7, 0.5, 0.7);
	// This requires more detalization and I want it to be lightweight.
	//GLfloat specular[] = { 1, 1, 1, 1 };
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	//glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 21);

	glCallList(display_lists[LIST_DEAD_END]);

	glFlush();
	SDL_GL_SwapBuffers();

	frames_drawn++;
}

void TheGame::CreateLists()
{
	// For now everything is hard-coded, model loading will come if need be.

	GLuint start_index = glGenLists(6);
	for(int i = 0; i < LIST_COUNT; i++)
		display_lists[i] = start_index + i;

	const float crn_off = 0.3; // TODO: tweak this
	const float trn_off = 0.15; // TODO: tweak this too

	// A wall
	glNewList(display_lists[LIST_WALL], GL_COMPILE);
	glBegin(GL_QUADS);
	for(float i = 0; i < 10; i += 0.2)
	{
		for(float j = 0; j < 5 - 2*crn_off; j += 0.2)
		{
			glVertex3f(0, 2.5 - crn_off - j, - i);
			glVertex3f(0, 2.5 - crn_off - j - 0.2, -i);
			glVertex3f(0, 2.5 - crn_off - j - 0.2, -i - 0.2);
			glVertex3f(0, 2.5 - crn_off - j, -i - 0.2);
		}
	}
	glEnd();
	glEndList();

	// A corner cut
	glNewList(display_lists[LIST_CORNER], GL_COMPILE);
	glBegin(GL_QUADS);
	for(float i = 0; i < 10; i += 0.2)
	{
		glVertex3f(crn_off, crn_off, -i);
		glVertex3f(0, 0, -i);
		glVertex3f(0, 0, -i - 0.2);
		glVertex3f(crn_off, crn_off, -i - 0.2);
	}
	glEnd();
	glEndList();

	// DUDE! The current normal is transformed by the modelview matrix
	// at the point of specifying a vertex. No. Freakin. Earlier.

	// Straight pass-thru
	glNewList(display_lists[LIST_STRAIGHT_PASS], GL_COMPILE);

	glNormal3f(1, 0, 0);
	glPushMatrix();
	glTranslatef(-2.5, 2.5, 0);
	glCallList(display_lists[LIST_WALL]);
	glNormal3f(-1, 0, 0);
	glTranslatef(5, 0, 0);
	glCallList(display_lists[LIST_WALL]);
	glPopMatrix();
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glNormal3f(1, 0, 0);
	glCallList(display_lists[LIST_WALL]);
	glTranslatef(5, 0, 0);
	glNormal3f(-1, 0, 0);
	glCallList(display_lists[LIST_WALL]);
	glPopMatrix();

	glNormal3f(0.7071, -0.7071, 0);
	glPushMatrix();
	glTranslatef(-2.5, 5 - crn_off, 0);
	glCallList(display_lists[LIST_CORNER]);
	glTranslatef(5, 0, 0);
	glScalef(-1, 1, 1);
	glCallList(display_lists[LIST_CORNER]);
	glPopMatrix();
	glPushMatrix();
	glNormal3f(-0.7071, 0.7071, 0);
	glTranslatef(2.5 - crn_off, 0, 0);
	glCallList(display_lists[LIST_CORNER]);
	glTranslatef(-5 + 2*crn_off, 0, 0);
	glScalef(-1, 1, 1);
	glCallList(display_lists[LIST_CORNER]);
	glPopMatrix();

	glEndList();

	// A dead-end
	glNewList(display_lists[LIST_DEAD_END], GL_COMPILE);
	glCallList(display_lists[LIST_STRAIGHT_PASS]);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	// crazy hack for the lazy
	for(float i = 0; i < 5; i += 0.2)
		for(float j = 0; j < 5; j += 0.2)
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
}

void TheGame::VideoInit()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER| SDL_INIT_AUDIO) < 0)
		throw MazeException(string("SDL initialization failed")+SDL_GetError());

	unsigned int flags = SDL_OPENGL | SDL_DOUBLEBUF;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// I don't know what is the big deal with those attributes and whether they
	// suit our needs. I'll research this later.|| don't try =)
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	if(SDL_SetVideoMode(settings.wnd_width, settings.wnd_height, 32, flags) == NULL)
		throw MazeException(string("Could not set video mode")+SDL_GetError());

	glViewport(0, 0, settings.wnd_width, settings.wnd_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, settings.wnd_width / settings.wnd_height, 0.1, 100.0);
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

	// Makes it look pretty funny
	//glPolygonMode(GL_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT, GL_LINE);

	CreateLists();
}

void TheSound::SoundInit()
{
	// Although ogg and mp3 functionality is nice, so far we're relying
	// on WAV only.
	int flags = MIX_INIT_OGG | MIX_INIT_MOD | MIX_INIT_MP3;
	Mix_Init(flags);

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
		throw MazeException(string("Unable to open audio device: ") + Mix_GetError());
}

void TheSound::PlaySound(const char* track, int loop)
{
	backMusic = Mix_LoadWAV(track);
	if(backMusic == NULL)
		printf("loading: %s\n", Mix_GetError());
	if(Mix_PlayChannel(1, backMusic, 1)==-1)// play on 1 channel.
		printf("Mix_PlayChannel: %s\n",Mix_GetError());
}

void TheGame::Cleanup()
{
	// I believe there's no harm in closing an audio device that is already
	// closed or consequentally calling two SDL_Quit(), and anyway this is
	// unlikely to happen.
	Mix_CloseAudio();
	SDL_Quit();
}
