#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <RayTriangleIntersection.h>
#include <DrawingWindow.h>
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
	std::vector<vector<uint32_t>> image;
	int col = 0;
	int row = 0;
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
	// std::cout << line << std::endl;
	std::getline(in, line);
	// std::cout << line << std::endl;
	in >> line;
	ppm.col = std::stoi(line);
	// std::cout << line << std::endl;
	in >> line;
	ppm.row = std::stoi(line);
	// std::cout << line << std::endl;
	in >> line;
	// std::cout << line << std::endl;

	for (int i = 0; i < ppm.row; i++)
	{
		vector<uint32_t> row;
		for (int j = 0; j < ppm.col; j++)
		{
			in >> red;
			in >> green;
			in >> blue;
			uint32_t pixel = (255 << 24) + (red << 16) + (green << 8) + blue;
			row.push_back(pixel);
		}
		ppm.image.push_back(row);
	}

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

ObjContent objRead(string filename)
{
	ObjContent toReturn = populatePalette("cornell-box.mtl");
	vector<vec3> vertices;
	string line;
	Colour currentCol = Colour(0, 0, 0);
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
				sscanf(chh, "f %i/ %i/ %i/", &A, &B, &C); //here it reads the line starting with f and store the corresponding values in the variables
				A--;
				B--;
				C--;
				ModelTriangle t = ModelTriangle(vertices.at(A), vertices.at(B), vertices.at(C), currentCol);
				toReturn.addFace(t);
			}
		}
	}
	return toReturn;
}
#endif