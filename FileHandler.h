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
#include <PpmContent.h>
using namespace std;
using namespace glm;

#define WIDTH 250
#define HEIGHT 250
#define DELTA 25
#define THETA 0.3
#define FOCAL_LENGTH HEIGHT / 2

int w = WIDTH / 2;
int h = HEIGHT / 2;

// logo camera pos
vec3 CAMERA_POS(200, 200, 400);
// cornell box camera pos
// vec3 CAMERA_POS(0, 3, 3);
mat3 CAMERA_ROT(1, 0, 0, 0, 1, 0, 0, 0, 1);
DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

class ObjContent
{
public:
	ObjContent()
	{
		/*
		we need pointers to the correct PpmContent in each Material.
		When you push_back to a vector, it may be resized which invalidates all pointers
		Because PpmContent is stored in a vector, we cannot let it change size.
		TERRIBLE PRACTICE FIX - we use reserve to cap to set the size of the vector.
		if we go over a size of 10, all ppm pointers invalid and textures won't work.
		under 10 - we waste a bunch of memory :(
		a better fix exists where we maintain pointers to pointers but i dont want to do it
		*/
		ppms.reserve(10);
	}
	std::vector<ModelTriangle> faces;
	std::vector<CanvasTriangle> textureTris;
	std::unordered_map<std::string, Material> palette;
	std::vector<PpmContent> ppms;
	int max = 0;

	void addFace(ModelTriangle face)
	{
		faces.push_back(face);
	}
	void addTextureTri(CanvasTriangle tex)
	{
		textureTris.push_back(tex);
	}

	void addMaterial(Material mtl, std::string name)
	{
		palette[name] = mtl;
	}
	PpmContent *addPpm(PpmContent ppm)
	{
		ppms.push_back(ppm);
		int index = ppms.size() - 1;
		PpmContent *ptr = &ppms[index];
		return ptr;
	}
};

// class ObjContentTexture
// {
// public:
// 	ObjContentTexture() {}
// 	std::vector<TextureTriangle> faces;
// 	// std::unordered_map<std::string, Colour> palette;
// 	// int max = 0;

// 	void addFace(TextureTriangle face)
// 	{
// 		faces.push_back(face);
// 	}
// };

void ppmWrite(PpmContent ppm, int n)
{
	char red;
	char green;
	char blue;
	uint32_t colour;
	string filename = "./outputs/frame" + to_string(n) + ".ppm";
	std::ofstream out(filename, std::ios::out);
	if (!out)
	{
		std::cerr << "Cannot open "
				  << "texture_out.ppm" << std::endl;
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
			// colour = ppm.image[i][j];
			colour = window.pixelBuffer[WIDTH * i + j];
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
	int red;
	int green;
	int blue;
	string line;
	PpmContent ppm;
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}

	std::getline(in, line);
	// std::getline(in, line);
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

ObjContent populatePalette(string filename)
{
	ObjContent content;

	ofstream myfile;
	string line;
	Material currentMtl = Material("none");
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
				if (currentMtl.name != "none")
				{
					content.addMaterial(currentMtl, currentMtl.name);
				}
				string *stuff = split(line, ' ');
				key = stuff[1];
				currentMtl = Material(key);
			}
			else if (line.substr(0, 2) == "Ka")
			{
				string *stuff = split(line, ' ');
				int r = 255 * stod(stuff[1]);
				int g = 255 * stod(stuff[2]);
				int b = 255 * stod(stuff[3]);
				currentMtl.setKaToColour(vec3(r, g, b));
			}
			else if (line.substr(0, 2) == "Kd")
			{
				string *stuff = split(line, ' ');
				int r = 255 * stod(stuff[1]);
				int g = 255 * stod(stuff[2]);
				int b = 255 * stod(stuff[3]);
				currentMtl.setKdToColour(vec3(r, g, b));
			}
			else if (line.substr(0, 2) == "Ks")
			{
				string *stuff = split(line, ' ');
				int r = 255 * stod(stuff[1]);
				int g = 255 * stod(stuff[2]);
				int b = 255 * stod(stuff[3]);
				currentMtl.setKsToColour(vec3(r, g, b));
			}
			else if (line.substr(0, 2) == "Ns")
			{
				string *stuff = split(line, ' ');
				currentMtl.setSpecularExponent(stof(stuff[1]));
			}
			else if (line.substr(0, 3) == "Mir")
			{
				string *stuff = split(line, ' ');
				float mi = stof(stuff[1]);
				currentMtl.mirrorness = mi;
			}
			else if (line.substr(0, 6) == "map_Ka")
			{
				string *stuff = split(line, ' ');
				PpmContent ppm = ppmRead(stuff[1]);
				PpmContent *ptr = content.addPpm(ppm);
				currentMtl.setKaToTexture(ptr);
			}
			else if (line.substr(0, 6) == "map_Kd")
			{
				string *stuff = split(line, ' ');
				PpmContent ppm = ppmRead(stuff[1]);
				PpmContent *ptr = content.addPpm(ppm);
				currentMtl.setKdToTexture(ptr);
			}
			else if (line.substr(0, 6) == "map_Ks")
			{
				string *stuff = split(line, ' ');
				PpmContent ppm = ppmRead(stuff[1]);
				PpmContent *ptr = content.addPpm(ppm);
				currentMtl.setKsToTexture(ptr);
			}
		}
	}

	content.addMaterial(currentMtl, currentMtl.name);
	myfile.close();
	return content;
}

ObjContent objRead(string filename)
{

	ObjContent toReturn;
	vector<vec3> vertices;
	vector<TexturePoint> texturePoints;
	string line;
	Material currentMtl;
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
				string *stuff = split(line, ' ');
				string mtlFname = stuff[1];
				toReturn = populatePalette(mtlFname);
			}
			if (line.substr(0, 6) == "usemtl")
			{
				string *stuff = split(line, ' ');
				currentMtl = toReturn.palette.at(stuff[1]);
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
				ModelTriangle tri = ModelTriangle(vertices.at(A), vertices.at(B), vertices.at(C), currentMtl);
				CanvasTriangle tex_tri = CanvasTriangle(texturePoints.at(At), texturePoints.at(Bt), texturePoints.at(Ct));
				toReturn.addTextureTri(tex_tri);
				toReturn.addFace(tri);
			}
		}
		n++;
	}
	return toReturn;
}

#endif