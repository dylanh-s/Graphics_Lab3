#ifndef CAMERA_H
#define CAMERA_H
#include "FileHandler.h"

using namespace std;
using namespace glm;

#define WIDTH 500
#define HEIGHT 500
#define DELTA 1
#define THETA 0.02
#define FOCAL_LENGTH HEIGHT / 2

int w = WIDTH / 2;
int h = HEIGHT / 2;

vec3 CAMERA_POS(0, 3, 3);
mat3 CAMERA_ROT(1, 0, 0, 0, 1, 0, 0, 0, 1);
DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

void translateCamera(float x, float y, float z);
void rotateCamera(float x, float y, float z);
void focusCamera(vec3 p);

void translateCamera(float x, float y, float z)
{
	CAMERA_POS += vec3(x, y, z);
}

void rotateCamera(float x, float y, float z)
{
	CAMERA_ROT *= mat3{cos(z) * cos(y), cos(z) * sin(y) * sin(x) - sin(z) * cos(x), cos(z) * sin(y) * cos(x) + sin(z) * sin(x),
					   sin(z) * cos(y), sin(z) * sin(y) * sin(x) + cos(z) * cos(x), sin(z) * sin(y) * cos(x) - cos(z) * sin(x),
					   -sin(y), cos(y) * sin(x), cos(y) * cos(x)};
}

void lookCamera(vec3 p)
{
	vec3 f = vec3(CAMERA_ROT[0][2], CAMERA_ROT[1][2], CAMERA_ROT[2][2]);
	vec3 forward = glm::normalize(f - p);
	vec3 up = vec3(CAMERA_ROT[0][1], CAMERA_ROT[1][1], CAMERA_ROT[2][1]);
	vec3 right = normalize(glm::cross(forward, up));
	CAMERA_ROT = transpose(mat3(right, up, forward));
}
#endif