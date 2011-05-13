// This file is released under the terms of the GNU General Public License v3.
// Copyright 2011  Danila Bespalov <danila.bespalov@gmail.com>,
//                 Gleb Devlikamov <onclip@gmail.com>
//
// Model handling.

#include <fstream>
#include <sstream>
#include "maze.h"


// Loads an OBJ model file, using only the basic syntax subset. Always assumes
// that for each facet all normals and texture coordinates are given. On files
// that don't meet this condition or are broken the behavior is undefined.
// Despite being so limited it's fine for our purposes.
bool MazeModel::LoadOBJ(const char* filename, float scale)
{
	ifstream fi(filename);
	if(!fi.is_open())
		throw MazeException(string("Cannot load object file: ") + filename);

	char c;
	string str, str2;
	// Sure, there's some memory overhead.
	vector<vertexdata> v;
	vector<vertexdata> vt;
	vector<vertexdata> vn;
	vertexdata tmp;
	vector<vertexdata> facet;
	stringstream ss, ss2;

	facets.clear();

	for(int ln = 1; fi.good(); ln++)
	{
		fi.get(c);
		switch(c)
		{
		case '\n':
			continue;
		case '#':
			break;
		case 'v':
			fi.get(c);
			switch(c)
			{
			case ' ':
				fi >> tmp.x;
				tmp.x *= scale;
				fi >> tmp.y;
				tmp.y *= scale;
				fi >> tmp.z;
				tmp.z *= scale;
				v.push_back(tmp);
				break;
			case 't':
				fi >> tmp.s;
				fi >> tmp.t;
				vt.push_back(tmp);
				break;
			case 'n':
				fi >> tmp.nx;
				fi >> tmp.ny;
				fi >> tmp.nz;
				vn.push_back(tmp);
				break;
			default:
				cerr << "Warning: can't parse line " << ln << " in " <<
					filename << endl;
				break;
			}
			break;
		case 'f':
			getline(fi, str);
			ss.clear();
			ss.str(str);
			while(true)
			{
				ss.clear();
				ss >> str2;
				if(ss.fail()) break;
				for(int i = 0; i < str2.size(); i++)
					if(str2[i] == '/') str2[i] = ' ';
				ss2.clear();
				ss2.str(str2);
				int idx;
				ss2 >> idx; // the vertex
				tmp.x = v[idx-1].x;
				tmp.y = v[idx-1].y;
				tmp.z = v[idx-1].z;
				ss2 >> idx; // texture coordinates
				tmp.s = vt[idx-1].s;
				tmp.t = vt[idx-1].t;
				ss2 >> idx; // the normal
				tmp.nx = vn[idx-1].nx;
				tmp.ny = vn[idx-1].ny;
				tmp.nz = vn[idx-1].nz;

				facet.push_back(tmp);
			}
			facets.push_back(facet);
			facet.clear();
			break;
		default:
			//cout << c << " is not supported\n";
			break;
		}

		// moving to the next line
		while(1)
		{
			fi.get(c);
			if(c == '\n' || !fi.good()) break;
		}
	}

	int num = 0;
	for(int i = 0; i < facets.size(); i++)
		num += facets[i].size();
	cout << filename << ": " << facets.size() << " facets, " << num <<
		" vertices\n";

	return true;
}

void MazeModel::Render()
{
	// TODO: rewrite using vertex arrays
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	for(int i = 0; i < facets.size(); i++)
	{
		glBegin(GL_POLYGON);
		for(vector<vertexdata>::const_iterator it = facets[i].begin();
				it != facets[i].end(); it++)
		{
			glNormal3f(it->nx, it->ny, it->nz);
			glTexCoord2f(it->s, it->t);
			glVertex3f(it->x, it->y, it->z);
		}
		glEnd();
	}
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}
