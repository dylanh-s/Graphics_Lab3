#ifndef OBJECT_H
#define OBJECT_H
#include "ModelTriangle.h"
#include "Material.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace glm;

class OBJ
{
public:
	OBJ()
	{
		ppms.reserve(20);
	}
	std::vector<ModelTriangle> faces;
	std::vector<TextureTriangle> textureTris;
	std::unordered_map<std::string, MTL> mtls;
	std::vector<PPM> ppms;
	int max = 0;

	void addFace(ModelTriangle face)
	{
		faces.push_back(face);
	}

	void addTextureTri(TextureTriangle tex)
	{
		textureTris.push_back(tex);
	}

	void addMaterial(MTL mtl, std::string name)
	{
		mtls[name] = mtl;
	}

	PPM *addTexture(PPM ppm)
	{
		ppms.push_back(ppm);
		int index = ppms.size() - 1;
		PPM *ptr = &ppms[index];
		return ptr;
	}
	void translateOBJ(float x, float y, float z, vector<int> f)
	{
		for (uint i = 0; i < f.size(); i++)
			for (uint j = 0; j < 3; j++)
				faces[f[i]].vertices[j] += vec3(x, y, z);
	}
	void rotateOBJ(float x, float y, float z, vector<int> f)
	{
		for (uint i = 0; i < f.size(); i++)
			for (uint j = 0; j < 3; j++)
				faces[f[i]].vertices[j] = mat3{cos(z) * cos(y), cos(z) * sin(y) * sin(x) - sin(z) * cos(x), cos(z) * sin(y) * cos(x) + sin(z) * sin(x),
					                           sin(z) * cos(y), sin(z) * sin(y) * sin(x) + cos(z) * cos(x), sin(z) * sin(y) * cos(x) - cos(z) * sin(x),
					                           -sin(y), cos(y) * sin(x), cos(y) * cos(x)} * faces[f[i]].vertices[j];
	}
	void scaleOBJ(float x, float y, float z, vector<int> f)
	{
		for (uint i = 0; i < f.size(); i++)
			for (uint j = 0; j < 3; j++)
				faces[f[i]].vertices[j] = mat3{x, 0, 0, 0, y, 0, 0, 0, z} * faces[f[i]].vertices[j];
	}
};
#endif