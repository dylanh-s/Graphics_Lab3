#ifndef CAMERA_H
#define CAMERA_H
#include "FileHandler.h"

using namespace std;
using namespace glm;

#define WIDTH 500
#define HEIGHT 500
#define DELTA 1
#define THETA 0.02
#define FOCAL_LENGTH 250.0

vec3 CAMERA_POS(0, 4, 5);
mat3 CAMERA_ROT(1, 0, 0, 0, 1, 0, 0, 0, 1);
DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

void lookAt(vec3 p);
CanvasPoint projectPoint(vec3 p);
CanvasPoint convertCoordinates(CanvasPoint p);

void lookAt(vec3 p)
{
	vec3 f = vec3(CAMERA_ROT[0][2], CAMERA_ROT[1][2], CAMERA_ROT[2][2]);
	vec3 forward = glm::normalize(f - p);
	vec3 up = vec3(CAMERA_ROT[0][1], CAMERA_ROT[1][1], CAMERA_ROT[2][1]);
	vec3 right = normalize(glm::cross(forward, up));
	CAMERA_ROT = transpose(mat3(right, up, forward));
}

CanvasPoint projectPoint(vec3 p)
{
	p = (p - CAMERA_POS) * CAMERA_ROT;
	
	return CanvasPoint((p.x / -p.z) * FOCAL_LENGTH, (p.y / p.z) * FOCAL_LENGTH, 1 / p.z);
}

CanvasPoint convertCoordinates(CanvasPoint p)
{
	int w = window.width / 2;
	int h = window.height / 2;
	float xp = w + (p.x);
	float yp = h + (p.y);
	return CanvasPoint(xp, yp, p.depth);
}

#endif