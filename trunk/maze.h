// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// The main header file for the project.

#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <cstdlib>
#include <SDL.h>
#include <sdl_opengl.h>
#include <SDL_image.h>
#include <AL/alut.h>
#include <iostream>


using namespace std;

// I'm so sick of writing the class name in front of enum ids, I'm declaring
// all enums top-level from now on. You don't wanna write things like
// if(keys[cfg.keys[MazeSettings::MOVE_FORTH]]), right?
enum {
	MOVE_FORTH=0,
	MOVE_BACK,
	MOVE_LEFT,
	MOVE_RIGHT
};

// Contains application settings
class MazeSettings
{
	public:

	MazeSettings();

	void Read();
	void Store();

	SDLKey keys[32]; // we'll hardly need any more.
	int wnd_width, wnd_height;
};

// Provides high-level sound access.
class TheSound
{

	public:

	TheSound()
	{
		buffer = AL_NONE;
		Allum_Ind = 0;
	}

	void SoundInit();
	void OpenSound(const char* track, int loop);
	void PlaySound();
	void StopSound();
	void DieSound();// Закрытие AL
	string GetErrAL()
	{
		return string("ALUT error: ") + alutGetErrorString(alutGetError());
	}

	ALuint buffer;
	ALuint source[30];//made of allumum
	ALenum error;
	ALint status;
	long long Allum_Ind;
	int audio_rate;
	int audio_channels;
	int audio_buffers;

};

class MazeModel
{
	public:
	//MazeModel();
	bool LoadOBJ(const char*);
	void Render();
	private:
	struct vertexdata
	{
		float x, y, z;
		float nx, ny, nz;
		float s, t;
	};
	vector< vector<vertexdata> > facets;
};

class TubeSection;

class TheVideo
{
	friend class TheGame; // XXX
	public:
	void Init();
	void Draw();
	void TexInit();
	GLuint GetTexture(const string&);

	// Idiosyncrasic names are not result of my ignorance towards the reader
	// but are rather result of me having no idea how else to call it.
	void RunThrough(TubeSection*, bool backwards=false);
	void CreateLists();

	// Display lists. This all doesn't look too good.
	static const short LIST_COUNT = 10;
	enum ListID {
		LIST_WALL=0,
		LIST_CORNER,
		LIST_WALL_BRANCH,
		LIST_STRAIGHT_PASS,
		LIST_RIGHT_TURN,
		LIST_LEFT_TURN,
		LIST_DEAD_END,
		LIST_RIGHT_BRANCH,
		LIST_LEFT_BRANCH,
		LIST_FORK
	};
	GLuint dlists[LIST_COUNT];

	private:

	TubeSection* start;
	map<string, GLuint> tex;
	// NOTE: This if for demonstration purposes only, this is NOT how model
	// handling should actually be done.
	MazeModel rat;
};

// Needed for collision detection
class PaintOverRect
{
	public:
	PaintOverRect(unsigned char _c, float _x, float _z, float _w, float _h)
	{
		x = _x; z = _z;
		w = _w; h = _h;
		c = _c;
	}
	// x and z coordinates in the ordinary OpenGL space,
	// w corresponds to the rect. size in the >X> direction, h is for >Z>.
	float x, z;
	float w, h;
	// the char is sort of a paintover's color
	// 0x0 - player can't be there
	// 0x1 - open space. visitors welcome
	// 0x10 - triggers moving to links[0]
	// 0x11 -                 to links[1]
	// 0x12 -                 to links[2]
	// 0x13 -                 to links[3]
	unsigned char c;
};

class TubeSection
{
	public:
	TubeSection()
	{
		rot = 0;
		trans.x = 0;
		trans.z = 0;
		for(int i = 0; i < 4; i++) links[i] = NULL;
	}
	void SetTransform(float _rot, float x, float z)
	{
		rot = _rot;
		trans.x = x;
		trans.z = z;
	}
	void Attach(); // firmly attaches right things at the right place

	enum SectionType {
		STRAIGHT_PASS=0,
		LEFT_BRANCH,
		RIGHT_BRANCH,
		LEFT_TURN,
		RIGHT_TURN,
		FORK,
		DEAD_END
	} type;
	GLuint list; // associated display list
	TubeSection* links[4]; // describes how the section is conneted with others
	float rot; // rotational tranformation
	struct { float x, z; } trans; // translation
	vector<PaintOverRect> paintover;
};

// We got only one.
class ThePlayer
{
		public:
	ThePlayer()
	{
		yaw = 0; pitch = 0;
		xpos = 0; ypos = 0; zpos = 0;
		walkbias = 0;
		walkbiasangle = 0;
		piover180 = 0.0174532925;;
	}
	bool Move(float, float, float);
	void ChangeLight(int);
	void Walk(float x, float y, float z);

	TubeSection* current_section;
	float walkbias;
	float walkbiasangle;
	float piover180;
	float heading;
	float yaw; // yaw is nose right, nose left
	float pitch; // pitch is nose up, nose down
	float xpos, ypos, zpos; // don't change directly, use Move()
};

// Does the main work like rendering, input processing, etc
// Do not create instances of this class, use TheGame::Get()
class TheGame
{
	public:

	TheGame()
	{
		if(m_instance == NULL) m_instance = this;
		should_stop = false;
		data_path = "Data/"; // we certainly need something more advanced
	}

	void MainLoop();
	int Run();
	void Cleanup();
	void ProcessEvents();

	static TheGame* Get()
	{
		return m_instance;
	}
	void SetSettings(MazeSettings _settings)
	{
		settings = _settings;
	}
	MazeSettings GetSettings()
	{
		return settings;
	}

	// helper classes
	TheVideo video;
	TheSound sound;
	ThePlayer player;

	string data_path;

	private:
	static TheGame* m_instance;
	bool should_stop;
	short frames_drawn;
	MazeSettings settings;
};

class MazeException
{
	public:
	MazeException(const string err, bool kill=false)
	{
		m_err = err;
		m_kill = kill;
	}
	string What()
	{
		return m_err;
	}
	bool ShouldTerminate()
	{
		return m_kill;
	}
	private:
	string m_err;
	bool m_kill;
};


void report_error(string);
TubeSection* maze_build(int, int);

// Some of the following may need tweaking
const float crn_off = 0.3; // how deeply the corners are cut
const float trn_off = 0.15; // the same only for the turns
const float bpu = 2; // TODO: what the hell is that?
const float brd_off = 0.8; // how close to a wall you can get

#endif // MAZE_H
