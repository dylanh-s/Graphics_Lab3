#include "ObjReader.h"
#include <ModelTriangle.h>
#include <Utils.h>

ObjReader::ObjReader(){};

ObjContent ObjReader::read(std::string f)
{
	ObjContent toReturn = ObjContent();
	toReturn.addFace(ModelTriangle());
	return toReturn;
};
