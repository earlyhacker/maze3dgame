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

	//glColor3f(0.7, 0.5, 0.7);
	//glColor3f(0.7, 0.7, 0.7);

	GLfloat light_pos[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	glRotatef(player.yaw, 0, cos(player.pitch * M_PI/180),
			sin(player.pitch * M_PI/180));
	glRotatef(player.pitch, 1, 0, 0);
	glTranslatef(-player.xpos, -player.ypos, -player.zpos);

	// TODO: move this out of here to TheSound and have it
	// called from another place.
	ALfloat orient_vec[] = { cos(player.yaw * M_PI/180), 0,
		-sin(player.yaw * M_PI/180), 0, 1, 0 };
	alListenerfv(AL_ORIENTATION, orient_vec);

	// There is a great deal of difference between an eager man who wants to
	// read a book and a tired man who wants a book to read.
	// (c) Chesterton, Essays

	player.current_section->Draw();
	if(player.current_section->links[0])
		RunThrough(player.current_section->links[0]);
	if(player.current_section->links[1])
		RunThrough(player.current_section->links[1]);
	if(player.current_section->links[3])
		RunThrough(player.current_section->links[3]);
	if(player.current_section->links[2])
		RunThrough(player.current_section, true);

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.05, -0.12, -0.29);
	glRotatef(180, 0, 1, 0);
	glRotatef(6, -1, 0, 0);
	glRotatef(8, 0, 1, 0);
	glColor3f(0.4, 0.4, 0.46);
	flashlight.Render();
	glPopMatrix();

	/*glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, 0.0, -15.3);
	glRotatef(player.yaw, 0, 1, 0);
	glRotatef(player.pitch, 1, 0, 0);
	rat.Render();
	glPopMatrix();*/

	glFlush();
	SDL_GL_SwapBuffers();
}

// Still can be improved but now it's at least of satisfactory quality.
// Working quality, not coding quality or readability.
void TheVideo::RunThrough(TubeSection* section, bool backwards)
{
	TubeSection* sec = section;
	glPushMatrix();
	if(!backwards)
	{
		glTranslatef(sec->trans.x, 0, sec->trans.z);
		glRotatef(sec->rot, 0, 1, 0);
		sec->Draw();
	}
	else
	{
		glRotatef(-sec->rot, 0, 1, 0);
		glTranslatef(-sec->trans.x, 0, -sec->trans.z);
		sec->links[2]->Draw();
		sec = sec->links[2];
	}
	while(1)
	{
		if(sec->links[1] && sec->links[1] != section)
		{
			glPushMatrix();
			glTranslatef(sec->links[1]->trans.x, 0, sec->links[1]->trans.z);
			glRotatef(sec->links[1]->rot, 0, 1, 0);
			sec->links[1]->Draw();
			glPopMatrix();
		}
		if(sec->links[3] && sec->links[3] != section)
		{
			glPushMatrix();
			glTranslatef(sec->links[3]->trans.x, 0, sec->links[3]->trans.z);
			glRotatef(sec->links[3]->rot, 0, 1, 0);
			sec->links[3]->Draw();
			glPopMatrix();
		}
		if(!backwards)
		{
			if(sec->links[0])
			{
				glTranslatef(sec->links[0]->trans.x, 0, sec->links[0]->trans.z);
				glRotatef(sec->links[0]->rot, 0, 1, 0);
				sec->links[0]->Draw();
				sec = sec->links[0];
				if(sec->type == TubeSection::FORK)
				{
					// I don't want another call to RunThrough() just for that
					glPushMatrix();
					glTranslatef(sec->links[1]->trans.x, 0,
							sec->links[1]->trans.z);
					glRotatef(sec->links[1]->rot, 0, 1, 0);
					sec->links[1]->Draw();
					glPopMatrix();

					glPushMatrix();
					glTranslatef(sec->links[3]->trans.x, 0,
							sec->links[3]->trans.z);
					glRotatef(sec->links[3]->rot, 0, 1, 0);
					sec->links[3]->Draw();
					glPopMatrix();
				}

			}
			else break;
		}
		else
		{
			if(sec->links[2])
			{
				if(sec->links[2]->links[0] != sec)
				{
					glRotatef(-sec->rot, 0, 1, 0);
					glTranslatef(-sec->trans.x, 0, -sec->trans.z);
					sec->links[2]->Draw();
					break;
				}
				glRotatef(-sec->rot, 0, 1, 0);
				glTranslatef(-sec->trans.x, 0, -sec->trans.z);
				sec->links[2]->Draw();
				sec = sec->links[2];
			}
			else break;
		}
	}
	glPopMatrix();
}

// Draws a plank with the given width and height, level of detail and the number
// of textures fitting vertically and horizontally. Origin is at the half-height
// of the plank. At lod=1.0 you have 8 rectangles per one unit, at lod=1.5 you
// have 12 and so on.
static void draw_plank(float w, float h, int wtex, int htex, float lod=1.0)
{
	float h_off = -h/2;
	lod *= 8;
	int hnum = (int)round(h * lod);
	int wnum = (int)round(w * lod);
	float dh = h / hnum;
	float dw = -w / wnum;
	glBegin(GL_QUADS);
	for(int i = 0; i < hnum; i++)
		for(int j = 0; j < wnum; j++)
		{
			glTexCoord2f(-wtex*j*dw/w, htex*i*dh/h);
			glVertex3f(0, h_off + i * dh, j * dw);

			glTexCoord2f(-wtex*(j+1)*dw/w, htex*i*dh/h);
			glVertex3f(0, h_off + i * dh, (j+1) * dw);

			glTexCoord2f(-wtex*(j+1)*dw/w, htex*(i+1)*dh/h);
			glVertex3f(0, h_off + (i+1) * dh, (j+1) * dw);

			glTexCoord2f(-wtex*j*dw/w, htex*(i+1)*dh/h);
			glVertex3f(0, h_off + (i+1) * dh, j * dw);
		}
	glEnd();
}

void TheVideo::CreateLists()
{
	flashlight.LoadOBJ("Data/flashlight.obj", 0.046);
	Uint32 started = SDL_GetTicks();

	GLuint start_index = glGenLists(6);
	for(int i = 0; i < LIST_COUNT; i++)
		dlists[i] = start_index + i;

	float d, d2; // d stands for delta

	GLuint wall = GetTexture("wall.jpg");
	//GLuint wall2 = GetTexture("wall2.jpg");

	// Having walls and the like in separate display lists helps to minimize
	// memory use.
	// A wall
	glNewList(dlists[LIST_WALL], GL_COMPILE);
	glColor3f(0.7, 0.7, 0.7);
	glBindTexture(GL_TEXTURE_2D, wall);
	draw_plank(10, 5 - 2*crn_off, 20, 10);
	glEndList();

	// A corner cut
	glNewList(dlists[LIST_CORNER], GL_COMPILE);
	glColor3f(0.7, 0.7, 0.7);
	glBindTexture(GL_TEXTURE_2D, wall);
	glPushMatrix();
	glTranslatef(crn_off/2.0, crn_off/2.0, 0);
	glRotatef(45, 0, 0, -1);
	glNormal3f(1, 0, 0);
	draw_plank(10, sqrt(2*crn_off*crn_off), 40, 2);
	glPopMatrix();
	glEndList();

	// DUDE! The current normal is transformed by the modelview matrix
	// at the point of specifying a vertex. No. Freakin. Earlier.

	// Straight pass-thru
	glNewList(dlists[LIST_STRAIGHT_PASS], GL_COMPILE);
	glColor3f(0.7, 0.7, 0.7);
	glBindTexture(GL_TEXTURE_2D, wall);

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

	glPushMatrix();
	glTranslatef(-2.5, 5 - crn_off, 0);
	glCallList(dlists[LIST_CORNER]);
	glTranslatef(5, 0, 0);
	glScalef(-1, 1, 1);
	glCallList(dlists[LIST_CORNER]);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2.5, crn_off, 0);
	glScalef(1, -1, 1);
	glCallList(dlists[LIST_CORNER]);
	glTranslatef(5, 0, 0);
	glScalef(-1, 1, 1);
	glCallList(dlists[LIST_CORNER]);
	glPopMatrix();

	glEndList();

	// A dead-end
	glNewList(dlists[LIST_DEAD_END], GL_COMPILE);
	glColor3f(0.7, 0.7, 0.7);
	glBindTexture(GL_TEXTURE_2D, wall);
	glCallList(dlists[LIST_STRAIGHT_PASS]);
	glPushMatrix();
	glTranslatef(-2.5, 2.5, -10);
	glRotatef(-90, 0, 1, 0);
	glNormal3f(1, 0, 0);
	draw_plank(5, 5, 10, 10);
	glPopMatrix();
	glEndList();

	// A block used to build two-way passages
	glNewList(dlists[LIST_WALL_BRANCH], GL_COMPILE);
	glColor3f(0.7, 0.7, 0.7);
	glBindTexture(GL_TEXTURE_2D, wall);
	glNormal3f(1, 0, 0);
	draw_plank(2.5 - trn_off, 5 - 2*crn_off, 5, 10);
	glPushMatrix();
	glTranslatef(0, 0, -2.5 + trn_off);
	glPushMatrix();
	glTranslatef(-trn_off/2.0, -2.5 + crn_off, -trn_off/2.0);
	glRotatef(90, 1, 0, 0);
	glRotatef(45, 0, 0, -1);
	draw_plank(5 - 2*crn_off, sqrt(2*trn_off*trn_off), 20, 1);
	glPopMatrix();

	// TODO: It should be a single block
	glBegin(GL_QUADS);
	d = (crn_off + trn_off) / 5;
	for(float i = 0; i < crn_off + trn_off; i += d)
	{
		glNormal3f(0, 1, 0);
		glTexCoord2f(0, 0);
		glVertex3f(crn_off - i, -2.5, -i);
		glTexCoord2f(1, 0);
		glVertex3f(crn_off, -2.5, -i);
		glTexCoord2f(1, 1);
		glVertex3f(crn_off, -2.5, -i - d);
		glTexCoord2f(0, 1);
		glVertex3f(crn_off - i -d, -2.5, -i - d);

		glNormal3f(0, -1, 0);
		glTexCoord2f(0, 0);
		glVertex3f(crn_off - i, 2.5, -i);
		glTexCoord2f(1, 0);
		glVertex3f(crn_off, 2.5, -i);
		glTexCoord2f(1, 1);
		glVertex3f(crn_off, 2.5, -i - d);
		glTexCoord2f(0, 1);
		glVertex3f(crn_off - i -d, 2.5, -i - d);
	}
	glEnd();

	// Way to go, the light will be broken on those
	glBegin(GL_QUADS);
	glNormal3f(cos(-M_PI/4)*cos(M_PI/4), sin(-M_PI/4), -cos(-M_PI/4)*sin(M_PI/4));
	glTexCoord2f(1, 1);
	glVertex3f(crn_off, 2.5, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0, 2.5 - crn_off, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-trn_off, 2.5 - crn_off, -trn_off);
	glTexCoord2f(1, 0);
	glVertex3f(-trn_off, 2.5, -trn_off - crn_off);

	glNormal3f(cos(M_PI/4)*cos(M_PI/4), sin(M_PI/4), -cos(M_PI/4)*sin(M_PI/4));
	glTexCoord2f(1, 1);
	glVertex3f(crn_off, -2.5, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0, -2.5 + crn_off, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-trn_off, -2.5 + crn_off, -trn_off);
	glTexCoord2f(1, 0);
	glVertex3f(-trn_off, -2.5, -trn_off - crn_off);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(crn_off/2.0, 2.5 - crn_off/2.0, 0);
	glRotatef(45, 0, 0, -1);
	glNormal3f(1, 0, 0);
	draw_plank(2.5 - trn_off, sqrt(2*crn_off*crn_off), 10, 2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(crn_off/2.0, -2.5 + crn_off/2.0, 0);
	glRotatef(-45, 0, 0, -1);
	draw_plank(2.5 - trn_off, sqrt(2*crn_off*crn_off), 10, 2);
	glPopMatrix();
	glEndList();

	// A passage with a left turn
	glNewList(dlists[LIST_LEFT_BRANCH], GL_COMPILE);
	glColor3f(0.7, 0.7, 0.7);
	glBindTexture(GL_TEXTURE_2D, wall);
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
	glTranslatef(-2.5 + crn_off/2 - trn_off/2, 0, -2.5 - crn_off);
	glRotatef(90, 0, 0, -1);
	glNormal3f(-1, 0, 0);
	draw_plank(5 - 2*crn_off, trn_off + crn_off, 20, 2);
	glTranslatef(-5, 0, 0);
	glNormal3f(1, 0, 0);
	draw_plank(5 - 2*crn_off, trn_off + crn_off, 20, 2);
	glPopMatrix();

	glEndList();

	glNewList(dlists[LIST_RIGHT_BRANCH], GL_COMPILE);
	glPushMatrix();
	glScalef(-1, 1, 1);
	glCallList(dlists[LIST_LEFT_BRANCH]);
	glPopMatrix();
	glEndList();

	// Left turn
	// TODO: get rid of scaling
	glNewList(dlists[LIST_LEFT_TURN], GL_COMPILE);
	glColor3f(0.7, 0.7, 0.7);
	glBindTexture(GL_TEXTURE_2D, wall);
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
	glTranslatef(2.5, 2.5, -7.5);
	glRotatef(90, 0, 1, 0);
	glNormal3f(-1, 0, 0);
	draw_plank(5 + trn_off, 5, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5 + crn_off/2 - trn_off/2, 0, -2.5 - crn_off);
	glRotatef(90, 0, 0, -1);
	glNormal3f(-1, 0, 0);
	draw_plank(5 - 2*crn_off, trn_off + crn_off, 20, 2);
	glTranslatef(-5, 0, 0);
	glNormal3f(1, 0, 0);
	draw_plank(5 - 2*crn_off, trn_off + crn_off, 20, 2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5 + crn_off, 2.5, -7.5);
	glBegin(GL_TRIANGLES);
	glNormal3f(1, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(0, -2.5, 0);
	glTexCoord2f(1, 1);
	glVertex3f(0, -2.5 + crn_off, 0);
	glTexCoord2f(1, 0);
	glVertex3f(0, -2.5, crn_off);
	glTexCoord2f(0, 0);
	glVertex3f(0, 2.5, 0);
	glTexCoord2f(1, 1);
	glVertex3f(0, 2.5 - crn_off, 0);
	glTexCoord2f(1, 0);
	glVertex3f(0, 2.5, crn_off);
	glEnd();
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, -2.5, 0);
	glPushMatrix();
	glTranslatef(-crn_off/2.0, crn_off/2.0, 0);
	glRotatef(45, 0, 0, -1);
	glNormal3f(-1, 0, 0);
	draw_plank(crn_off + trn_off, sqrt(2*crn_off*crn_off), 2, 2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-crn_off/2.0, 5 - crn_off/2.0, 0);
	glRotatef(45, 0, 0, 1);
	draw_plank(crn_off + trn_off, sqrt(2*crn_off*crn_off), 2, 2);
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5 - trn_off, 2.5, -2.5);
	glRotatef(90, 0, 1, 0);
	glNormal3f(1, 0, 0);
	draw_plank(2.5 - trn_off, 5 - 2*crn_off, 5, 10);
	glPushMatrix();
	glTranslatef(5, 0, 0);
	glNormal3f(-1, 0, 0);
	draw_plank(2.5 - trn_off, 5 - 2*crn_off, 5, 10);
	glTranslatef(-2.5, -2.5, 0);
	glRotatef(90, 0, 0, 1);
	glNormal3f(1, 0, 0);
	draw_plank(2.5 - trn_off, 5 - 2*crn_off, 5, 10);
	glTranslatef(5, 0, 0);
	glNormal3f(-1, 0, 0);
	draw_plank(2.5 - trn_off, 5 - 2*crn_off, 5, 10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(crn_off/2.0, 2.5 - crn_off/2.0, 0);
	glRotatef(45, 0, 0, -1);
	glNormal3f(1, 0, 0);
	draw_plank(2.5 - trn_off, sqrt(2*crn_off*crn_off), 10, 2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(crn_off/2.0, -2.5 + crn_off/2.0, 0);
	glRotatef(45, 0, 0, 1);
	draw_plank(2.5 - trn_off, sqrt(2*crn_off*crn_off), 10, 2);
	glPopMatrix();
	glTranslatef(5, 0, 0);
	glPushMatrix();
	glTranslatef(-crn_off/2.0, 2.5 - crn_off/2.0, 0);
	glRotatef(45, 0, 0, 1);
	glNormal3f(-1, 0, 0);
	draw_plank(2.5 - trn_off, sqrt(2*crn_off*crn_off), 10, 2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-crn_off/2.0, -2.5 + crn_off/2.0, 0);
	glRotatef(45, 0, 0, -1);
	draw_plank(2.5 - trn_off, sqrt(2*crn_off*crn_off), 10, 2);
	glPopMatrix();

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

	glNewList(dlists[LIST_START], GL_COMPILE);
	glPushMatrix();
	glTranslatef(0, 0, -10);
	glScalef(1, 1, -1);
	glCallList(dlists[LIST_DEAD_END]);
	glPopMatrix();
	glEndList();

	cout << "Lists created, took " << SDL_GetTicks() - started << " ms.\n";
}

void TheVideo::Init()
{
	const MazeSettings& cfg = TheGame::Get()->GetSettings();

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER| SDL_INIT_AUDIO) < 0)
		throw MazeException(string("SDL initialization failed")+SDL_GetError());
	atexit(SDL_Quit);

	unsigned int flags = SDL_OPENGL | SDL_DOUBLEBUF;
	if(cfg.fullscreen) flags |= SDL_FULLSCREEN;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	if(SDL_SetVideoMode(cfg.wnd_width, cfg.wnd_height, 32, flags) == NULL)
		throw MazeException(string("Could not set video mode") +
				SDL_GetError(), true);

	glViewport(0, 0, cfg.wnd_width, cfg.wnd_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, cfg.wnd_width / cfg.wnd_height, 0.1, 45.0);
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
	GLfloat no_ambient[] = { 0.07, 0.07, 0.07, 1 }; // well, almost
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, no_ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	glEnable(GL_FOG);
	glFogf(GL_FOG_DENSITY, 0.096);

	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	CreateLists();
	start = maze_build(cfg.difficulty, cfg.difficulty);
	ThePlayer& player = TheGame::Get()->player;
	player.ypos = 2.5;
	player.zpos = -1.5;
	player.current_section = start;
	player.ChangeLight(3);
}

// Returns an OpenGL texture ID if it's already loaded or loads it otherwise.
// Gets the name of the texture which is the name of a file in Data/tex/ folder.
GLuint TheVideo::GetTexture(const string& name)
{
	if(tex.count(name)) return tex[name];

	GLuint tex_id;
	SDL_Surface* orig_img;
	SDL_Surface* tex_img;
	SDL_PixelFormat* fmt = new SDL_PixelFormat; // 32 bit BGRA

	fmt->BitsPerPixel = 32;
	fmt->BytesPerPixel = 4;
	fmt->Bmask = 0x000000ff;
	fmt->Gmask = 0x0000ff00;
	fmt->Rmask = 0x00ff0000;
	fmt->Amask = 0xff000000;
	fmt->palette = NULL;

	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	orig_img = IMG_Load((TheGame::Get()->data_path + "/tex/" + name).c_str());
	if(orig_img == NULL)
		throw MazeException(string("Could not load texture: ") + name);
	tex_img = SDL_ConvertSurface(orig_img, fmt, SDL_SWSURFACE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, tex_img->w, tex_img->h, GL_BGRA,
	//		GL_UNSIGNED_BYTE, tex_img->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, tex_img->w, tex_img->h, 0, GL_BGRA,
			GL_UNSIGNED_BYTE, tex_img->pixels);

	SDL_FreeSurface(orig_img);
	SDL_FreeSurface(tex_img);


	tex[name] = tex_id;
	return tex_id;
}

void FinalSection::Draw()
{
	glCallList(list);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.2, 0.94, 0.2, 0.26);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(-2.5, 5, -6);
	glVertex3f(-2.5, 0, -6);
	glVertex3f(2.5, 0, -6);
	glVertex3f(2.5, 5, -6);
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

StartSection::StartSection(const TubeSection& sec) : TubeSection(sec)
{
	paintover.push_back(PaintOverRect(0x0, -2.5, -brd_off, 5, brd_off));
}

void StartSection::Draw()
{
	glPushMatrix();
	glNormal3f(1, 0, 0);
	glTranslatef(2.5, 2.5, 0);
	glRotatef(90, 0, 1, 0);
	glColor3f(0.7, 0.7, 0.7);
	draw_plank(5, 5, 10, 10);
	glPopMatrix();
	glCallList(list);
}
