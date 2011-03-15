// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Contains rendering and video initialization code.

#include "maze.h"

SDL_Surface* TheGame::screen;
SDL_Color clr = {255,50,40,0};
SDL_Rect dest = {80, 120,0,0};
SDL_Surface* screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); 

void print_ttf(SDL_Surface *sDest, char* message, char* font, int size, 
		SDL_Color color, SDL_Rect dest)
	{
		TTF_Font *fnt = TTF_OpenFont(font, size);
		SDL_Surface *sText = TTF_RenderText_Blended( fnt, message, color);
		SDL_BlitSurface( sText,NULL, sDest,&dest );
		SDL_FreeSurface( sText );
		TTF_CloseFont( fnt );
	}

void TheGame::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	TTF_Init();
	atexit(TTF_Quit);

	//print_ttf(TheGame::screen, "hello!", "font.ttf", 16, clr, dest);
	clr.r = 255;
	clr.g = 255;
	clr.b = 0;
	dest.x = 80;
	dest.y = 250;
	//print_ttf(TheGame::screen, "hello!", "font.ttf", 16, clr, dest);
	//SDL_Flip(screen);
	// Lost of matrix stuff right ahead.
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
	glRotatef(yaw, 0.0, 1.0, 0.0);
	glRotatef(-pitch, 1.0, 0.0, 0.0);
	glTranslatef(-0.5, -0.5, 0.5);

	// That's how the display lists are used.
	// In this case we create a list temporarily and delete it afterwards.
	GLuint face_list = glGenLists(1);
	glNewList(face_list, GL_COMPILE);
	glBegin(GL_QUADS);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	glEnd();
	glEndList();

	// Front
	glColor3f(0.6, 0.2, 0.2);
	glCallList(face_list);
	// Back
	glColor3f(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -1.0);
	glCallList(face_list);
	glPopMatrix();
	// Left
	glColor3f(0.3, 0.3, 0.65);
	glPushMatrix();
	glRotatef(90, 0.0, 1.0, 0.0);
	glCallList(face_list);
	glPopMatrix();
	// Right
	glColor3f(0.65, 0.3, 0.65);
	glPushMatrix();
	glTranslatef(1.0, 0.0, 0.0);
	glRotatef(90, 0.0, 1.0, 0.0);
	glCallList(face_list);
	glPopMatrix();
	// Top
	glColor3f(0.75, 0.75, 0.2);
	glPushMatrix();
	glTranslatef(0.0, 1.0, 0.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(face_list);
	glPopMatrix();
	// Bottom
	glColor3f(0.3, 0.67, 0.15);
	glPushMatrix();
	glRotatef(-90, 1.0, 0.0, 0.0);
	glCallList(face_list);
	glPopMatrix();

	glDeleteLists(face_list, 1);

	glFlush();
	SDL_GL_SwapBuffers();

	frames_drawn++;
}

void TheGame::VideoInit()
{
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
}

void TheGame::SoundInit()
{
	int flags=MIX_INIT_OGG|MIX_INIT_MOD|MIX_INIT_MP3;
	int initted=Mix_Init(flags);
	if(initted&flags != flags) {
    printf("Mix_Init: Failed to init required ogg and mod support!\n");
    printf("Mix_Init: %s\n", Mix_GetError());
    // handle error
	}
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
    printf("Mix_OpenAudio: %s\n", Mix_GetError());
    exit(2);}

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)){
		printf("Unable to open audio!\n");
		atexit(SDL_Quit);}

	Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);

}



/*

	if(SDL_Init(SDL_INIT_AUDIO) < 0)
		printf("TTF_Init: %s\n");
	
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)){
		printf("Unable to open audio!\n");
		atexit(SDL_Quit);}

	Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
*/