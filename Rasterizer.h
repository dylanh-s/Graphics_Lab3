#ifndef RAZTERIZER_H
#define RAZTERIZER_H
#include "Camera.h"

using namespace std;
using namespace glm;

CanvasPoint projectPoint(vec3 p);
CanvasTriangle projectTriangle(ModelTriangle t);
std::vector<CanvasPoint> calculateLine(CanvasPoint start, CanvasPoint end);
void drawLine(CanvasPoint start, CanvasPoint end, Colour colour);
void drawStrokedTriangle(CanvasTriangle tri);
void drawFilledTriangle(CanvasTriangle tri);
void drawFrame(OBJ obj);
void drawRaster(OBJ obj);
void drawTexture(PPM ppm);

CanvasPoint projectPoint(vec3 p)
{
	p = (p - CAMERA_POS) * CAMERA_ROT;
	return CanvasPoint((p.x / -p.z) * FOCAL_LENGTH + w, (p.y / p.z) * FOCAL_LENGTH + h, 1 / p.z);
}

CanvasTriangle projectTriangle(ModelTriangle tri)
{
	return CanvasTriangle(projectPoint(tri.vertices[0]), projectPoint(tri.vertices[1]), projectPoint(tri.vertices[2]), Colour(tri.mtl.Kd));
}

std::vector<CanvasPoint> calculateLine(CanvasPoint start, CanvasPoint end)
{
	std::vector<CanvasPoint> points;
	float xDiff = end.x - start.x;
	float yDiff = end.y - start.y;
	float zDiff = end.depth - start.depth;
	float numberOfSteps = std::max((std::max(abs(xDiff), abs(yDiff))), abs(zDiff));
	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;
	float zStepSize = zDiff / numberOfSteps;
	for (float i = 0.0; i < numberOfSteps; i++)
	{
		float z = start.depth + (zStepSize * i);
		float x = round(start.x + (xStepSize * i));
		float y = round(start.y + (yStepSize * i));
		points.push_back(CanvasPoint(x, y, z));
	}

	return points;
}

void drawLine(CanvasPoint start, CanvasPoint end, Colour colour)
{
	std::vector<CanvasPoint> points = calculateLine(start, end);
	for (uint i = 0; i < points.size(); i++)
	{
		window.setPixelColour(points[i].x, points[i].y, points[i].depth, colour.pack());
	}
}

void drawStrokedTriangle(CanvasTriangle tri)
{
	drawLine(tri.vertices[0], tri.vertices[1], tri.colour);
	drawLine(tri.vertices[1], tri.vertices[2], tri.colour);
	drawLine(tri.vertices[2], tri.vertices[0], tri.colour);
}

void drawFilledTriangle(CanvasTriangle tri)
{
	if (tri.vertices[1].y < tri.vertices[0].y)
	{
		swap(tri.vertices[0], tri.vertices[1]);
	}
	if (tri.vertices[2].y < tri.vertices[1].y)
	{
		swap(tri.vertices[2], tri.vertices[1]);
	}
	if (tri.vertices[1].y < tri.vertices[0].y)
	{
		swap(tri.vertices[0], tri.vertices[1]);
	}
	std::vector<vector<CanvasPoint>> lines;
	lines.push_back(calculateLine(tri.vertices[0], tri.vertices[2]));
	lines.push_back(calculateLine(tri.vertices[0], tri.vertices[1]));
	lines.push_back(calculateLine(tri.vertices[1], tri.vertices[2]));

	drawStrokedTriangle(tri);
	uint d = 0;
	for (uint i = 1; i < 3; i++)
	{
		for (uint j = 0; j < lines[i].size(); j++)
		{
			while (lines[0][d].y != lines[i][j].y && d != (lines[0].size() - 1))
			{
				if (lines[0][d].y > lines[i][j].y)
				{
					d--;
				}
				else if (lines[0][d].y < lines[i][j].y)
				{
					d++;
				}
			}
			if (j == 0)
			{
				drawLine(lines[i][j], lines[0][d], tri.colour);
			}
			else if (lines[i][j].y != lines[i][j - 1].y)
			{
				drawLine(lines[i][j], lines[0][d], tri.colour);
			}
		}
	}
}

void drawFrame(OBJ obj)
{
#pragma omp parallel
#pragma omp for
	for (uint i = 0; i < obj.faces.size(); i++)
	{
		drawStrokedTriangle(projectTriangle(obj.faces.at(i)));
	}
}

void drawRaster(OBJ obj)
{
#pragma omp parallel
#pragma omp for
	for (uint i = 0; i < obj.faces.size(); i++)
	{
		drawFilledTriangle(projectTriangle(obj.faces.at(i)));
	}
}

void drawTexture(PPM ppm)
{
	for (int i = 0; i < ppm.height; i++)
	{
		for (int j = 0; j < ppm.width; j++)
		{
			window.setPixelColour(j, i, 0, ppm.image[i][j]);
		}
	}
}
#endif