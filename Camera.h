#ifndef CAMERA_H
#define CAMERA_H
#include "FileHandler.h"

using namespace std;
using namespace glm;

void translateCamera(float x, float y, float z);
void revolveCamera(float x, float y, float z);
void rotateCamera(float x, float y, float z);
void lookCamera(vec3 p);
void orbitCamera(vec3 o);

void translateCamera(float x, float y, float z)
{
	cameraPosition += vec3(x, y, z);
}

void revolveCamera(float x, float y, float z)
{
	cameraPosition = mat3{cos(z) * cos(y), cos(z) * sin(y) * sin(x) - sin(z) * cos(x), cos(z) * sin(y) * cos(x) + sin(z) * sin(x),
					      sin(z) * cos(y), sin(z) * sin(y) * sin(x) + cos(z) * cos(x), sin(z) * sin(y) * cos(x) - cos(z) * sin(x),
					      -sin(y), cos(y) * sin(x), cos(y) * cos(x)} * cameraPosition;
}

void rotateCamera(float x, float y, float z)
{
	cameraRotation *= mat3{cos(z) * cos(y), cos(z) * sin(y) * sin(x) - sin(z) * cos(x), cos(z) * sin(y) * cos(x) + sin(z) * sin(x),
					       sin(z) * cos(y), sin(z) * sin(y) * sin(x) + cos(z) * cos(x), sin(z) * sin(y) * cos(x) - cos(z) * sin(x),
					       -sin(y), cos(y) * sin(x), cos(y) * cos(x)};
}

void lookCamera(vec3 p)
{
	vec3 forward = normalize(cameraPosition - p);
	vec3 right = glm::cross(normalize(vec3(0.0, 1.0, 0.0)), forward);
	vec3 up = glm::cross(forward, right);
	cameraRotation = mat3(right, up, forward);
}

void orbitCamera(vec3 o)
{	
	float x = cameraPosition[0];
	float y = cameraPosition[1];
	float z = cameraPosition[2];
	float rho = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	float theta = atan(y / sqrt(pow(x, 2) + pow(z, 2)));
	float phi = atan2(x, z);
	vec3 orbit = vec3(rho, theta, phi) + o;
	x = orbit[0] * cos(orbit[1]) * sin(orbit[2]);
	z = orbit[0] * cos(orbit[1]) * cos(orbit[2]);
	y = orbit[0] * sin(orbit[1]);
	cameraPosition = vec3(x, y, z);
	lookCamera(vec3(0,0,0));
}
#endif