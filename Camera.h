#ifndef CAMERA_H
#define CAMERA_H
#include "FileHandler.h"

using namespace std;
using namespace glm;

void translateCamera(float x, float y, float z);
void rotateCamera(float x, float y, float z);
void lookCamera(vec3 p);

void translateCamera(float x, float y, float z)
{
	cameraPosition += vec3(x, y, z);
}

void rotateCamera(float x, float y, float z)
{
	cameraRotation *= mat3{cos(z) * cos(y), cos(z) * sin(y) * sin(x) - sin(z) * cos(x), cos(z) * sin(y) * cos(x) + sin(z) * sin(x),
					   sin(z) * cos(y), sin(z) * sin(y) * sin(x) + cos(z) * cos(x), sin(z) * sin(y) * cos(x) - cos(z) * sin(x),
					   -sin(y), cos(y) * sin(x), cos(y) * cos(x)};
}

void lookCamera(vec3 p)
{
	vec3 f = vec3(cameraRotation[0][2], cameraRotation[1][2], cameraRotation[2][2]);
	vec3 forward = glm::normalize(f - p);
	vec3 up = vec3(cameraRotation[0][1], cameraRotation[1][1], cameraRotation[2][1]);
	vec3 right = normalize(glm::cross(forward, up));
	cameraRotation = transpose(mat3(right, up, forward));
}
#endif