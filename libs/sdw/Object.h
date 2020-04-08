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
		ppms.reserve(10);
	}
	std::vector<ModelTriangle> faces;
	std::vector<CanvasTriangle> textureTris;
	std::unordered_map<std::string, MTL> mtls;
	std::vector<PPM> ppms;
	int max = 0;

	void addFace(ModelTriangle face)
	{
		faces.push_back(face);
	}

	void addTextureTri(CanvasTriangle tex)
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
};
#endif