#include <ctime>
#include <vector>
#include "maze.h"

// To avoid possible future name collisions. Isn't that exactly
// what namespaces are for?
namespace maze_gen
{

class MazeCell
{
	public:
	MazeCell()
	{
		left = true;
		right = true;
		top = true;
		bottom = true;
		visited = false;
	}
	bool left, right, top, bottom, visited;
};

class Point
{
	public:
	Point(int a, int b)
	{
		x = a;
		y = b;
	}
	int x;
	int y;
};

// I vaguely remember seeing something similar in STL, but hey,
// this one got << and >> operators!
template <class T>
class Stack
{
	public:
	void operator<<(T a)
	{
		vec.push_back(a);
	}
	bool operator>>(T& a)
	{
		if(vec.empty()) return false;
		a = vec.back();
		vec.pop_back();
		return true;
	}
	bool Peep(T& a)
	{
		if(vec.empty()) return false;
		a = vec.back();
		return true;
	}
	int GetCount()
	{
		return vec.size();
	}
	private:
	vector<T> vec; // no oferflows guaranteed
};

static bool is_dead_end(Point, MazeCell**);
static bool is_ok(Point);

// Not that I like globals that much, but this code comes from a program that
// I wrote where they're used and I'm too lazy to fix this. Little harm, if any.
static int maze_width;
static int maze_height;

}
using namespace maze_gen;


TubeSection* maze_build(int width, int height)
{
	// TODO: Nobody likes mazes without exits!

	maze_height = height;
	maze_width = width;

	srand(time(NULL));

	// by convention we're using maze[x][y]
	MazeCell** cells;
	Stack<maze_gen::Point> stack;
	Stack<TubeSection*> sec_stack;
	Stack<int> dir_stack;

	cells = new MazeCell*[maze_width];
	for(int i = 0; i < maze_width; i++)
		cells[i] = new MazeCell[maze_height];

	int starting_point = rand() % maze_width;
	cells[starting_point][0].top = false;

	maze_gen::Point cur(starting_point, 0); // cur for current
	// All directions, when encoded by integers, represent the number of
	// counter-clockwise 90° turns to be done to face the right direction.
	int cur_dir = 2;
	TubeSection* cur_sec = new TubeSection;
	for(;;)
	{
		cells[cur.x][cur.y].visited = true;
		maze_gen::Point tmp = cur;
		if(is_dead_end(cur, cells))
		{
			bool res = stack >> cur;
			if(!res) break;
			dir_stack >> cur_dir;
			sec_stack >> cur_sec;
		}
		else
		{
			int dir = rand() % 4;
			switch(dir)
			{
			case 0:
				tmp.y--;
				break;
			case 2:
				tmp.y++;
				break;
			case 3:
				tmp.x++;
				break;
			case 1:
				tmp.x--;
				break;
			}
			if(is_ok(tmp))
			{
				if(!cells[tmp.x][tmp.y].visited)
				{
					// Remove the wall and start from there.
					switch(dir)
					{
					case 0:
						cells[cur.x][cur.y].top = false;
						cells[tmp.x][tmp.y].bottom = false;
						break;
					case 1:
						cells[cur.x][cur.y].bottom = false;
						cells[tmp.x][tmp.y].top = false;
						break;
					case 2:
						cells[cur.x][cur.y].right = false;
						cells[tmp.x][tmp.y].left = false;
						break;
					case 3:
						cells[cur.x][cur.y].left = false;
						cells[tmp.x][tmp.y].right = false;
						break;
					}
					stack << cur;
					cur = tmp;

					// Dealing with TubeSection stuff
					TubeSection* new_sec = new TubeSection;
					int local_dir = (dir - cur_dir + 4) % 4;
					cur_sec->links[local_dir] = new_sec;
					new_sec->links[2] = cur_sec;
					cur_sec->Attach();
					new_sec->Attach();

					dir_stack << cur_dir;
					cur_dir = dir;
					sec_stack << cur_sec;
					cur_sec = new_sec;
				}
			}
		}
	}

	return cur_sec;
}

bool maze_gen::is_ok(maze_gen::Point p)
{
	return (p.x >= 0 && p.x < maze_width && p.y >= 0 && p.y < maze_height);
}

bool maze_gen::is_dead_end(maze_gen::Point p, MazeCell** cells)
{
	// It's just... insanely efficient. With vectors and all
	vector<maze_gen::Point> pts;
	maze_gen::Point tmp(0,0);

	tmp = p;
	tmp.x++;
	if(is_ok(tmp)) pts.push_back(tmp);
	tmp = p;
	tmp.x--;
	if(is_ok(tmp)) pts.push_back(tmp);
	tmp = p;
	tmp.y++;
	if(is_ok(tmp)) pts.push_back(tmp);
	tmp = p;
	tmp.y--;
	if(is_ok(tmp)) pts.push_back(tmp);

	bool ret = true;
	for(int i = 0; i < pts.size(); i++)
		ret &= cells[pts[i].x][pts[i].y].visited;

	return ret;
}

void TubeSection::Attach()
{
	GLuint* dlists = TheGame::Get()->video.dlists;
	paintover.clear();
	if(links[1] && links[3] && !links[0])
	{
		type = FORK;
		list = dlists[TheVideo::LIST_FORK];
		links[1]->SetTransform(90, -5, -2.5 - trn_off);
		links[3]->SetTransform(-90, 5, -2.5 - trn_off);

		paintover.push_back(PaintOverRect(0x1, -5, -5 - trn_off, 10, 5));
		paintover.push_back(PaintOverRect(0x1, -2.5, -trn_off, 5, trn_off));
		paintover.push_back(PaintOverRect(0x12, -2.5, 0, 5, 1));
		paintover.push_back(PaintOverRect(0x11, -6, -5 - trn_off, 1, 5));
		paintover.push_back(PaintOverRect(0x13, 5, -5 - trn_off, 1, 5));
	}
	else if(links[1] && !links[0])
	{
		type = LEFT_TURN;
		list = dlists[TheVideo::LIST_LEFT_TURN];
		links[1]->SetTransform(90, -5, -5);

		paintover.push_back(PaintOverRect(0x1, -2.5, -7.5, 5, 7.5));
		paintover.push_back(PaintOverRect(0x1, -5, -7.5, 2.5, 5));
		paintover.push_back(PaintOverRect(0x12, -2.5, 0, 5, 1));
		paintover.push_back(PaintOverRect(0x11, -6, -7.5, 1, 5));
	}
	else if(links[3] && !links[0])
	{
		type = RIGHT_TURN;
		list = dlists[TheVideo::LIST_RIGHT_TURN];
		links[3]->SetTransform(-90, 5, -5);

		paintover.push_back(PaintOverRect(0x1, -2.5, -7.5, 5, 7.5));
		paintover.push_back(PaintOverRect(0x1, 2.5, -7.5, 2.5, 5));
		paintover.push_back(PaintOverRect(0x12, -2.5, 0, 5, 1));
		paintover.push_back(PaintOverRect(0x13, 5, -7.5, 1, 5));
	}
	else if(links[1] && links[0])
	{
		type = LEFT_BRANCH;
		list = dlists[TheVideo::LIST_LEFT_BRANCH];
		links[1]->SetTransform(90, -2.5 - trn_off, -5);
		links[0]->SetTransform(0, 0, -10);

		paintover.push_back(PaintOverRect(0x1, -2.5, -10, 5, 10));
		paintover.push_back(PaintOverRect(0x1, -2.5 - trn_off, -7.5,
					trn_off, 5));
		paintover.push_back(PaintOverRect(0x12, -2.5, 0, 5, 1));
		paintover.push_back(PaintOverRect(0x10, -2.5, -11, 5, 1));
		paintover.push_back(PaintOverRect(0x11, -3.5, -7.5, 1, 5));
	}
	else if(links[3] && links[0])
	{
		type = RIGHT_BRANCH;
		list = dlists[TheVideo::LIST_RIGHT_BRANCH];
		links[3]->SetTransform(-90, 2.5 + trn_off, -5);
		links[0]->SetTransform(0, 0, -10);

		paintover.push_back(PaintOverRect(0x1, -2.5, -10, 5, 10));
		paintover.push_back(PaintOverRect(0x1, 2.5, -7.5, trn_off, 5));
		paintover.push_back(PaintOverRect(0x12, -2.5, 0, 5, 1));
		paintover.push_back(PaintOverRect(0x10, -2.5, -11, 5, 1));
		paintover.push_back(PaintOverRect(0x13, 2.5, -7.5, 1, 5));
	}
	else if(links[0] && !links[1] && !links[3])
	{
		type = STRAIGHT_PASS;
		list = dlists[TheVideo::LIST_STRAIGHT_PASS];
		links[0]->SetTransform(0, 0, -10);

		paintover.push_back(PaintOverRect(0x1, -2.5, -10, 5, 10));
		paintover.push_back(PaintOverRect(0x12, -2.5, 0, 5, 1));
		paintover.push_back(PaintOverRect(0x10, -2.5, -11, 5, 1));
	}
	else if( !(links[0] && links[1] && links[3]) )
	{
		type = DEAD_END;
		list = dlists[TheVideo::LIST_DEAD_END];

		paintover.push_back(PaintOverRect(0x1, -2.5, -10, 5, 10));
		paintover.push_back(PaintOverRect(0x12, -2.5, 0, 5, 1));
	}
	else
	{
		throw MazeException("TubeSection::Attach(): Unknown linking configuration");
	}
}
