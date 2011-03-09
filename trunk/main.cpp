#include "Variables.h"
#include "Action.h"
#include "Lists.h"


void ProcessEvents()
{

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			exit(0);
			break;
		case SDL_KEYDOWN:
			 switch(event.key.keysym.sym)
                   {
                     case SDLK_ESCAPE:
						 exit(0);
						 break;
                    }
                break;
		}
	}
}

bool BoundingBox(float a, float b, float c, float d)
{	
	float vert[4] ={a, b, c, d};
	float big1 =0; 
	float big2 =0;
	int fix;
	for(int i =0; i <= 15; i++)
		if (vert[i] > big1){ big1 = vert[i]; fix = i;}
	for(int i =0; i <=15; i++)
		if (vert[i] > big2 && i !=i )big2=vert[i];
	return false;
}
void Draw ()
{

	GLfloat x_m, y_m, z_m, u_m, v_m;
	GLfloat xtrans = -xpos;
	GLfloat ztrans = -zpos;
	GLfloat ytrans = -walkbias-0.25f;
	GLfloat sceneroty = 360.0f - yrot;
	
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	///
	glRotatef(lookupdown,1.0f,0,0);
	glRotatef(sceneroty,0,1.0f,0);
	glTranslatef(xtrans, ytrans, ztrans);
	//glRotatef(90, 0, 0, 1.0f);
	
	glColor3f(0.0f,0.7f, 0.0f);
	BoxList();
	//glEnable(GL_LIGHT0);       
	//glEnable(GL_LIGHTING);  // Включает освещение
	//glEnable(GL_COLOR_MATERIAL);

	for (int i =0; i <=6; i++)
	{
		glTranslatef(0.0f, 0.0f, 2.0f);
		glCallList(Floor);
		
	}
	for(int i =0; i <=3; i++)
	{
		glTranslatef(2.0f, 0.0f, 0.0f);
		glCallList(Floor);
	}
		

}

bool Run()
{	
	for(;;)
	{
		ProcessEvents();
		
		int *mx;
		int *my;
		Uint8* key_state = SDL_GetKeyState(NULL);
		if (key_state[SDLK_UP]) UpStep();
		if (key_state[SDLK_DOWN]) DownStep();
		if (key_state[SDLK_RIGHT]){heading -= 1.0f; yrot = heading;}
		if (key_state[SDLK_LEFT]){heading += 1.0f; yrot = heading;}
		if (key_state[SDLK_HOME]){lookupdown++;}
		if (key_state[SDLK_END]){lookupdown--;}

		while (SDL_GetTicks() - LastTime < 1000.0f / 60.0f)
		{
			SDL_Delay(1);
		}

		Draw();
		SDL_GL_SwapBuffers();
		LastTime = SDL_GetTicks();
	}
	return true;
}

bool Init()
{
	
	const SDL_VideoInfo *VideoInfo = NULL;
	int flag = 0;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL initialization failed %s\n", SDL_GetError());
		return false;
	}
	atexit(SDL_Quit);
	VideoInfo = SDL_GetVideoInfo();
	if (VideoInfo == NULL)
	{
		fprintf(stderr, "Attempt to get video info failed %s\n", SDL_GetError());
		return false;
	}
	flag = SDL_OPENGL;
	if (isFullscreen)
		flag |= SDL_FULLSCREEN;
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_SetVideoMode(ScreenWidth, ScreenHeight,
		VideoInfo->vfmt->BitsPerPixel,flag);//SDL_DOUBLEBUF|SDL_HWSURFACE|

	glViewport(0, 0, ScreenWidth, ScreenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//SDL_ShowCursor(SDL_DISABLE);
	gluPerspective(45.0f, ScreenWidth/ ScreenHeight, 0.1f, 100.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glMatrixMode(GL_MODELVIEW);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_POINTS);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	
	return true;
}

int main(int argc, char **argv)
{
	Init();
	Run();

	return 0;
} 

/*  Код кубика, никогда не знаешь когда оно пригодится =)
		 glVertex3f(-rib, -rib,  rib);
		 glVertex3f( rib, -rib,  rib);
		 glVertex3f( rib,  rib,  rib);
		 glVertex3f(-rib,  rib,  rib);
		// Back Face
		
		 glVertex3f(-rib, -rib, -rib);
		 glVertex3f(-rib,  rib, -rib);
		 glVertex3f( rib,  rib, -rib);
		 glVertex3f( rib, -rib, -rib);
		// Top Face
		 glVertex3f(-rib,  rib, -rib);
		 glVertex3f(-rib,  rib,  rib);
		 glVertex3f( rib,  rib,  rib);
		 glVertex3f( rib,  rib, -rib);
		// Bottom Face
		
		 glVertex3f(-rib, -rib, -rib);
		 glVertex3f( rib, -rib, -rib);
		 glVertex3f( rib, -rib,  rib);
		 glVertex3f(-rib, -rib,  rib);
		// Right face
		
		 glVertex3f( rib, -rib, -rib);
		 glVertex3f( rib,  rib, -rib);
		 glVertex3f( rib,  rib,  rib);
		 glVertex3f( rib, -rib,  rib);
		// Left Face
		
		 glVertex3f(-1.0f, -rib, -rib);
		 glVertex3f(-1.0f, -rib,  rib);
		 glVertex3f(-1.0f,  rib,  rib);
		 glVertex3f(-1.0f,  rib, 
