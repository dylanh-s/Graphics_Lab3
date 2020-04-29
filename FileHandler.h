#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include "DrawingWindow.h"
#include "ModelTriangle.h"
#include "CanvasTriangle.h"
#include "TextureTriangle.h"
#include "Intersection.h"
#include "Object.h"
#include "Utils.h"
#include <glm/glm.hpp>
#include <fstream>

using namespace std;
using namespace glm;

#define DELTA 25
#define THETA 0.1
#define WIDTH 480
#define HEIGHT 640
#define DEPTH 1150
#define FOCAL_LENGTH 625

OBJ obj;
int mode = 0;
int w = WIDTH / 2;
int h = HEIGHT / 2;
vector<vec3> lights;
vec3 cameraPosition(0, 0, DEPTH);
mat3 cameraRotation(1, 0, 0, 0, 1, 0, 0, 0, 1);
DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

void ppmWrite(int n);
PPM ppmRead(string filename);
OBJ mtlRead(string filename);
OBJ objRead(string filename);

void ppmWrite(int n)
{
	char red;
	char green;
	char blue;
	uint32_t pixel;
	string filename = "./outputs/frame" + to_string(n) + ".ppm";
	std::ofstream out(filename, std::ios::out | std::ios::binary);
	if (!out)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}

	out << "P6\n";
	out << "# Created by Alex & Dylan\n";
	out << to_string(WIDTH) << " " << to_string(HEIGHT) << "\n";
	out << "255\n";
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			pixel = window.pixelBuffer[WIDTH * i + j];
			red = char((pixel & 0x00FF0000) >> 16);
			green = char((pixel & 0x0000FF00) >> 8);
			blue = char((pixel & 0x000000FF));
			out << red << green << blue;
		}
	}
	out.close();
}

PPM ppmRead(string filename)
{
	int red;
	int green;
	int blue;
	string line;
	PPM ppm;
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}

	std::getline(in, line);
	std::getline(in, line);
	in >> line;
	ppm.width = std::stoi(line);
	in >> line;
	ppm.height = std::stoi(line);
	in >> line;
	ppm.colour = std::stoi(line);
	std::getline(in, line);
	for (int i = 0; i < ppm.height; i++)
	{
		vector<uint32_t> row;
		for (int j = 0; j < ppm.width; j++)
		{
			red = int(in.get());
			green = int(in.get());
			blue = int(in.get());
			uint32_t pixel = (int(255) << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			row.push_back(pixel);
		}
		ppm.image.push_back(row);
	}
	in.close();
	return ppm;
}

OBJ mtlRead(string filename)
{
	OBJ obj;
	MTL mtl = MTL("none");
	string line;
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}
	string key;
	while (std::getline(in, line))
	{
		if (line.size() > 4)
		{
			if (line.substr(0, 6) == "newmtl")
			{
				if (mtl.name != "none")
				{
					obj.addMaterial(mtl, mtl.name);
				}
				string *token = split(line, ' ');
				key = token[1];
				mtl = MTL(key);
			}
			else if (line.substr(0, 2) == "Ka")
			{
				string *token = split(line, ' ');
				int r = 255 * stod(token[1]);
				int g = 255 * stod(token[2]);
				int b = 255 * stod(token[3]);
				mtl.setKaToColour(vec3(r, g, b));
			}
			else if (line.substr(0, 2) == "Kd")
			{
				string *token = split(line, ' ');
				int r = 255 * stod(token[1]);
				int g = 255 * stod(token[2]);
				int b = 255 * stod(token[3]);
				mtl.setKdToColour(vec3(r, g, b));
			}
			else if (line.substr(0, 2) == "Ks")
			{
				string *token = split(line, ' ');
				int r = 255 * stod(token[1]);
				int g = 255 * stod(token[2]);
				int b = 255 * stod(token[3]);
				mtl.setKsToColour(vec3(r, g, b));
			}
			else if (line.substr(0, 2) == "Ns")
			{
				string *token = split(line, ' ');
				mtl.setSpecularity(stof(token[1]));
			}
			else if (line.substr(0, 3) == "Mir")
			{
				string *token = split(line, ' ');
				float mi = stof(token[1]);
				mtl.mirrorness = mi;
			}
			else if (line.substr(0, 6) == "map_Ka")
			{
				string *token = split(line, ' ');
				PPM ppm = ppmRead(token[1]);
				PPM *ptr = obj.addTexture(ppm);
				mtl.setKaToTexture(ptr);
			}
			else if (line.substr(0, 6) == "map_Kd")
			{
				string *token = split(line, ' ');
				PPM ppm = ppmRead(token[1]);
				PPM *ptr = obj.addTexture(ppm);
				mtl.setKdToTexture(ptr);
			}
			else if (line.substr(0, 6) == "map_Ks")
			{
				string *token = split(line, ' ');
				PPM ppm = ppmRead(token[1]);
				PPM *ptr = obj.addTexture(ppm);
				mtl.setKsToTexture(ptr);
			}
		}
	}
	obj.addMaterial(mtl, mtl.name);
	return obj;
}

OBJ objRead(string filename)
{
	OBJ obj;
	MTL mtl;
	vector<vec3> vertices;
	vector<TexturePoint> texturePoints;
	string line;
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}
	int n = 0;
	while (std::getline(in, line))
	{
		//check v for vertices
		if (line.size() > 4)
		{
			if (line.substr(0, 6) == "mtllib")
			{
				string *token = split(line, ' ');
				string mtlFname = token[1];
				obj = mtlRead(mtlFname);
			}
			if (line.substr(0, 6) == "usemtl")
			{
				string *token = split(line, ' ');
				mtl = obj.mtls.at(token[1]);
			}
			else if (line.substr(0, 2) == "v ")
			{
				string *token = split(line, ' ');
				double x = stod(token[1]);
				double y = stod(token[2]);
				double z = stod(token[3]);
				vertices.push_back(vec3(x, y, z));
			}
			else if (line.substr(0, 3) == "vt ")
			{
				string *token = split(line, ' ');
				double xt = stod(token[1]);
				double yt = stod(token[2]);
				texturePoints.push_back(TexturePoint(xt, yt));
			}
			//check for faces
			else if (line.substr(0, 2) == "f ")
			{
				int A, B, C;	//to store vertex indices
				int At, Bt, Ct; //to store texture indices
				const char *chh = line.c_str();
				sscanf(chh, "f %i/%i %i/%i %i/%i", &A, &At, &B, &Bt, &C, &Ct);
				At--;
				Bt--;
				Ct--;
				A--;
				B--;
				C--;
				ModelTriangle tri = ModelTriangle(vertices.at(A), vertices.at(B), vertices.at(C), mtl);
				TextureTriangle tex_tri = TextureTriangle(texturePoints.at(At), texturePoints.at(Bt), texturePoints.at(Ct));
				obj.addTextureTri(tex_tri);
				obj.addFace(tri);
			}
		}
		n++;
	}
	return obj;
}
#endif