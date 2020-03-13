#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <RayTriangleIntersection.h>
#include <DrawingWindow.h>
#include <TextureTriangle.h>
#include <glm/glm.hpp>
#include <Utils.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

using namespace std;
using namespace glm;

class PpmContent
{
public:
	PpmContent() {}
	std::vector<std::vector<uint32_t>> image;
	int width = 0;
	int height = 0;
	int colour = 0;
};

class ObjContent
{
public:
	ObjContent() {}
	std::vector<ModelTriangle> faces;
	std::unordered_map<std::string, Colour> palette;
	int max = 0;

	void addFace(ModelTriangle face)
	{
		faces.push_back(face);
	}

	void addColour(Colour col, std::string name)
	{
		palette[name] = col;
	}
};

class ObjContentTexture
{
public:
	ObjContentTexture() {}
	std::vector<TextureTriangle> faces;
	// std::unordered_map<std::string, Colour> palette;
	// int max = 0;

	void addFace(TextureTriangle face)
	{
		faces.push_back(face);
	}
};

void ppmWrite(PpmContent ppm)
{
	char red;
	char green;
	char blue;
	char data[3];
	uint32_t colour;
	std::ofstream out("testing.ppm", std::ios::out);
	if (!out)
	{
		std::cerr << "Cannot open " << "texture_out.ppm" << std::endl;
		exit(1);
	}

	out << "P6\n";
	out << "# Created by Alex & Dylan\n";
	out << to_string(ppm.width) << " " << to_string(ppm.height) << "\n";
	out << to_string(ppm.colour) << "\n";
	for (int i = 0; i < ppm.height; i++)
	{
		for (int j = 0; j < ppm.width; j++)
		{
			colour = ppm.image[i][j];
			red = (char)((colour & 0x00FF0000) >> 16);
			green = (char)((colour & 0x0000FF00) >> 8);
			blue = (char)((colour & 0x000000FF));
			out << red << green << blue;
		}
	}
	out << " ";
	out.close();
}
PpmContent ppmRead(string filename)
{
	char red;
	char green;
	char blue;
	string line;
	PpmContent ppm;
	std::ifstream in(filename, std::ios::in);
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
			in.get(red);
			in.get(green);
			in.get(blue);
			uint32_t pixel = (255 << 24) + (red << 16) + (green << 8) + blue;
			row.push_back(pixel);
		}
		ppm.image.push_back(row);
	}
	in.close();
	return ppm;
}

ObjContent populatePalette(string filename)
{
	ObjContent content;
	ofstream myfile;
	string line;
	Colour currentCol = Colour(0, 0, 0);
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
				string *stuff = split(line, ' ');
				key = stuff[1];
				currentCol = content.palette[key];
			}
			else if (line.substr(0, 2) == "Kd")
			{
				string *stuff = split(line, ' ');
				int r = 255 * stod(stuff[1]);
				int g = 255 * stod(stuff[2]);
				int b = 255 * stod(stuff[3]);
				content.addColour(Colour(r, g, b), key);
			}
		}
	}
	return content;
}

string getTextureFileName(string filename)
{
	string line;
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}
	while (std::getline(in, line))
	{
		if (line.substr(0, 6) == "map_Kd ")
		{
			string *stuff = split(line, ' ');
			return (stuff[1]);
		}
	}
	return "";
}

ObjContentTexture objReadTexture(string filename)
{

	ObjContentTexture toReturn;
	vector<vec3> vertices;
	vector<TexturePoint> texturePoints;
	string line;
	string currentTextureFname = "";
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}
	while (std::getline(in, line))
	{
		//check v for vertices
		if (line.size() > 4)
		{
			if (line.substr(0, 6) == "mtllib")
			{
				string *stuff = split(line, ' ');
				string mtlFname = stuff[1];
				currentTextureFname = getTextureFileName(mtlFname);
			}
			else if (line.substr(0, 2) == "v ")
			{
				string *stuff = split(line, ' ');
				double x = stod(stuff[1]);
				double y = stod(stuff[2]);
				double z = stod(stuff[3]);
				vertices.push_back(vec3(x, y, z));
			}
			else if (line.substr(0, 3) == "vt ")
			{
				string *stuff = split(line, ' ');
				double xt = stod(stuff[1]);
				double yt = stod(stuff[2]);
				texturePoints.push_back(TexturePoint(xt, yt));
			}
			//check for faces
			else if (line.substr(0, 2) == "f ")
			{
				int A, B, C;	//to store vertex indices
				int At, Bt, Ct; //to store texture indices
				const char *chh = line.c_str();
				sscanf(chh, "f %i/%i %i/%i %i/%i", &A, &At, &B, &Bt, &C, &Ct); //here it reads the line starting with f and store the corresponding values in the variables
				At--;
				Bt--;
				Ct--;
				A--;
				B--;
				C--;
				TextureTriangle t = TextureTriangle(vertices.at(A), vertices.at(B), vertices.at(C), texturePoints.at(At), texturePoints.at(Bt), texturePoints.at(Ct), currentTextureFname);
				toReturn.addFace(t);
			}
		}
	}
	return toReturn;
}

ObjContent objRead(string filename)
{

	ObjContent toReturn = populatePalette("cornell-box.mtl");
	vector<vec3> vertices;
	vector<vec2> texturePoints;
	string line;
	Colour currentCol = Colour(255, 255, 255);
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}
	while (std::getline(in, line))
	{
		//check v for vertices
		if (line.size() > 4)
		{
			if (line.substr(0, 6) == "usemtl")
			{
				string *stuff = split(line, ' ');
				string key = stuff[1];
				//std::cout << key << std::endl;
				currentCol = toReturn.palette[key];
			}
			else if (line.substr(0, 2) == "v ")
			{
				string *stuff = split(line, ' ');
				double x = stod(stuff[1]);
				double y = stod(stuff[2]);
				double z = stod(stuff[3]);
				vertices.push_back(vec3(x, y, z));
			}
			//check for faces
			else if (line.substr(0, 2) == "f ")
			{
				int A, B, C; //to store vertex indices
				const char *chh = line.c_str();
				ModelTriangle t;
				sscanf(chh, "f %i/ %i/ %i/", &A, &B, &C); //here it reads the line starting with f and store the corresponding values in the variables
				A--;
				B--;
				C--;
				t = ModelTriangle(vertices.at(A), vertices.at(B), vertices.at(C), currentCol);

				toReturn.addFace(t);
			}
		}
	}
	return toReturn;
}
#endif