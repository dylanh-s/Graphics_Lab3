#include "Rasterizer.h"
#include "Raytracer.h"

using namespace std;
using namespace glm;

void update();
void draw(PpmContent ppm, ObjContent obj);
void handleEvent(SDL_Event event);
vector<double> interpolate(double from, double to, int numberOfValues);
vector<vec3> interpolate3D(vec3 from, vec3 to, int numberOfValues);
CanvasTriangle getRandomTriangle();

int main(int argc, char *argv[])
{
	SDL_Event event;
	PpmContent ppm = ppmRead("texture.ppm");
	ObjContent obj = objRead("cornell-box.obj");

	while (true)
	{
		if (window.pollForInputEvents(&event))
			handleEvent(event);
		update();
		draw(ppm, obj);
		window.renderFrame();
	}
}

vector<double> interpolate(double from, double to, int numberOfValues)
{
	vector<double> v;

	double step = (to - from) / (numberOfValues - 1);

	v.push_back(from);

	for (int i = 0; i < numberOfValues - 1; i++)

	{
		v.push_back(v.back() + step);
	}
	return v;
}

vector<vec3> interpolate3D(vec3 from, vec3 to, int numberOfValues)
{
	vector<vec3> v;

	float sf = (float)(numberOfValues - 1);
	vec3 step = (to - from) / sf;

	v.push_back(from);

	for (int i = 0; i < numberOfValues - 1; i++)
	{
		v.push_back(v.back() + step);
	}
	return v;
}

vec3 colourToVec3(uint32_t rgb)
{
	int r = (rgb >> 16) & 0xFF;
	int g = (rgb >> 8) & 0xFF;
	int b = rgb & 0xFF;

	return vec3(r, g, b);
}

uint32_t vec3ToColour(vec3 rgb)
{
	uint32_t colour = (255 << 24) + (int(rgb.x) << 16) + (int(rgb.y) << 8) + int(rgb.z);
	return colour;
}

CanvasTriangle getRandomTriangle()
{
	bool allDifferent = false;
	CanvasPoint v0;
	CanvasPoint v1;
	CanvasPoint v2;
	while (!allDifferent)
	{
		v0 = CanvasPoint(rand() % window.width, rand() % window.height);
		v1 = CanvasPoint(rand() % window.width, rand() % window.height);
		v2 = CanvasPoint(rand() % window.width, rand() % window.height);
		allDifferent = true;
		if ((v0.x == v1.x && v0.y == v1.y) || (v0.x == v2.x && v0.y == v2.y) || (v1.x == v2.x && v1.y == v2.y))
		{
			allDifferent = false;
		}
	}
	
	return CanvasTriangle(v0, v1, v2, Colour(rand() % 255, rand() % 255, rand() % 255));
}

void update()
{
	// Function for performing animation (shifting artifacts or moving the camera)
}

void draw(PpmContent ppm, ObjContent obj)
{
	// drawRaster(obj);
	drawRaytrace(obj);
}

void handleEvent(SDL_Event event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_LEFT)
		{
			// cout << "LEFT" << endl;
		}
		else if (event.key.keysym.sym == SDLK_RIGHT)
		{
			// cout << "RIGHT" << endl;
		}
		else if (event.key.keysym.sym == SDLK_UP)
		{
			// cout << "UP" << endl;
		}
		else if (event.key.keysym.sym == SDLK_DOWN)
		{
			// cout << "DOWN" << endl;
		}
		else if (event.key.keysym.sym == SDLK_c)
		{
			window.clearPixels();
		}
		else if (event.key.keysym.sym == SDLK_s)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x, CAMERA_POS.y + DELTA, CAMERA_POS.z);
		}
		else if (event.key.keysym.sym == SDLK_w)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x, CAMERA_POS.y - DELTA, CAMERA_POS.z);
		}
		else if (event.key.keysym.sym == SDLK_a)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x + DELTA, CAMERA_POS.y, CAMERA_POS.z);
		}
		else if (event.key.keysym.sym == SDLK_d)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x - DELTA, CAMERA_POS.y, CAMERA_POS.z);
		}
		else if (event.key.keysym.sym == SDLK_q)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z + DELTA);
		}
		else if (event.key.keysym.sym == SDLK_e)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z - DELTA);
		}
		else if (event.key.keysym.sym == SDLK_t)
		{
			window.clearPixels();
			mat3 X_ROT = {1, 0, 0, 0, cos(THETA), -sin(THETA), 0, sin(THETA), cos(THETA)};
			CAMERA_ROT = CAMERA_ROT * X_ROT;
		}
		else if (event.key.keysym.sym == SDLK_g)
		{
			window.clearPixels();
			mat3 X_ROT = {1, 0, 0, 0, cos(THETA), -sin(THETA), 0, sin(THETA), cos(THETA)};
			CAMERA_ROT = CAMERA_ROT * glm::inverse(X_ROT);
		}
		else if (event.key.keysym.sym == SDLK_y)
		{
			window.clearPixels();
			mat3 Y_ROT = {cos(THETA), 0, sin(THETA), 0, 1, 0, -sin(THETA), 0, cos(THETA)};
			CAMERA_ROT = CAMERA_ROT * Y_ROT;
		}
		else if (event.key.keysym.sym == SDLK_h)
		{
			window.clearPixels();
			mat3 Y_ROT = {cos(THETA), 0, sin(THETA), 0, 1, 0, -sin(THETA), 0, cos(THETA)};
			CAMERA_ROT = CAMERA_ROT * glm::inverse(Y_ROT);
		}
		else if (event.key.keysym.sym == SDLK_u)
		{
			window.clearPixels();
			mat3 Z_ROT = {cos(THETA), -sin(THETA), 0, sin(THETA), cos(THETA), 0, 0, 0, 1};
			CAMERA_ROT = CAMERA_ROT * Z_ROT;
		}
		else if (event.key.keysym.sym == SDLK_j)
		{
			window.clearPixels();
			mat3 Z_ROT = {cos(THETA), -sin(THETA), 0, sin(THETA), cos(THETA), 0, 0, 0, 1};
			CAMERA_ROT = CAMERA_ROT * glm::inverse(Z_ROT);
		}
		else if (event.key.keysym.sym == SDLK_l)
		{
			window.clearPixels();
			lookAt(vec3(0.415989, 5.218497, -3.567968));
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		// cout << "MOUSE CLICKED" << endl;
	}
}