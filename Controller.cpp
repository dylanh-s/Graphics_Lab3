// #include "Rasterizer.h"
#include "Raytracer.h"

using namespace std;
using namespace glm;

int MODE = 2;

void update();
void draw(OBJ obj);
void handleEvent(SDL_Event event);

int main(int argc, char *argv[])
{
	SDL_Event event;
	OBJ obj = objRead("./inputs/logo.obj");
	int n = 1;
	while (true)
	{
		if (window.pollForInputEvents(&event))
			handleEvent(event);
		update();
		draw(obj);
		window.renderFrame();
		if (n <= 1)
			ppmWrite(n);
		cout << "frame" << n << "\n";
		n++;
	}
}

void update()
{
	// Function for performing animation (shifting artifacts or moving the camera)
}

void draw(OBJ obj)
{
	if (MODE == 0)
	{
		// drawFrame(obj);
	}
	else if (MODE == 1)
	{
		// drawRaster(obj);
	}
	else if (MODE == 2)
	{
		// solid shadows
		drawRaytrace(obj, MODE);
	}
	else if (MODE == 3)
	{
		// soft shadows
		// and anti-aliasing
		drawRaytraceWithAA(obj, MODE);
	}
	else if (MODE == 4)
	{
		// no shadows
		drawRaytrace(obj, MODE);
	}
	else if (MODE == 69)
	{
		// no shadows
		drawRaytrace(obj, MODE);
		// ObjContentTexture objTex = objReadTexture("logo.obj");
	}
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
		else if (event.key.keysym.sym == SDLK_m)
		{
			window.clearPixels();
			MODE = (MODE + 1) % 5;
		}
		else if (event.key.keysym.sym == SDLK_n)
		{
			window.clearPixels();
			MODE = (MODE - 1) % 5;
		}
		else if (event.key.keysym.sym == SDLK_a)
		{
			window.clearPixels();
			translateCamera(DELTA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_d)
		{
			window.clearPixels();
			translateCamera(-DELTA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_s)
		{
			window.clearPixels();
			translateCamera(0, DELTA, 0);
		}
		else if (event.key.keysym.sym == SDLK_w)
		{
			window.clearPixels();
			translateCamera(0, -DELTA, 0);
		}
		else if (event.key.keysym.sym == SDLK_q)
		{
			window.clearPixels();
			translateCamera(0, 0, DELTA);
		}
		else if (event.key.keysym.sym == SDLK_e)
		{
			window.clearPixels();
			translateCamera(0, 0, -DELTA);
		}
		else if (event.key.keysym.sym == SDLK_t)
		{
			window.clearPixels();
			rotateCamera(THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_g)
		{
			window.clearPixels();
			rotateCamera(-THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_y)
		{
			window.clearPixels();
			rotateCamera(0, THETA, 0);
		}
		else if (event.key.keysym.sym == SDLK_h)
		{
			window.clearPixels();
			rotateCamera(0, -THETA, 0);
		}
		else if (event.key.keysym.sym == SDLK_u)
		{
			window.clearPixels();
			rotateCamera(0, 0, THETA);
		}
		else if (event.key.keysym.sym == SDLK_j)
		{
			window.clearPixels();
			rotateCamera(0, 0, -THETA);
		}
		else if (event.key.keysym.sym == SDLK_l)
		{
			window.clearPixels();
			lookCamera(vec3(0.415989, 5.218497, -3.567968));
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		// cout << "MOUSE CLICKED" << endl;
	}
}