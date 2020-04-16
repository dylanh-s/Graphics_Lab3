#include "Rasterizer.h"
#include "Raytracer.h"

using namespace std;
using namespace glm;

void update(int n);
void draw(OBJ obj);
void handleEvent(SDL_Event event);

int main(int argc, char *argv[])
{
	int n = 0;
	SDL_Event event;
	OBJ obj = objRead("./inputs/logo6.obj");
	while (true)
	{
		if (window.pollForInputEvents(&event))
			handleEvent(event);
		update(n);
		draw(obj);
		window.renderFrame();
		n++;
	}
}

void update(int n)
{
	// cout << "frame" << n << "\n";
	if (n == 0)
	{
		mode = 1;
		// lights.push_back(vec3(412.000000, 230.000000, 100.000000));
		lights.push_back(vec3(0.0, 0.0, 100.0));
	}
	else if (n == 1)
	{
		ppmWrite(n);
	}
}

void draw(OBJ obj)
{
	if (mode == 0)      // Frame
	{
		drawFrame(obj);
	}
	else if (mode == 1) // Raster
	{
		drawRaster(obj);
	}
	else if (mode == 2) // Raytrace
	{
		drawRaytrace(obj);
	}
	else if (mode == 3) // Raytrace + hard shadows
	{
		drawRaytrace(obj);
	}
	else if (mode == 4) // Raytrace + soft shadows
	{
		drawRaytrace(obj);
	}
	else if (mode == 5) // Raytrace + soft shadows + anti-aliasing
	{
		drawRaytrace(obj);
	}
}

void handleEvent(SDL_Event event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_c)
		{
			window.clearPixels();
		}
		else if (event.key.keysym.sym == SDLK_m)
		{
			window.clearPixels();
			mode = (mode + 1) % 6;
		}
		else if (event.key.keysym.sym == SDLK_n)
		{
			window.clearPixels();
			mode = (mode - 1) % 6;
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
		else if (event.key.keysym.sym == SDLK_g)
		{
			window.clearPixels();
			rotateCamera(0, THETA, 0);
		}
		else if (event.key.keysym.sym == SDLK_j)
		{
			window.clearPixels();
			rotateCamera(0, -THETA, 0);
		}
		else if (event.key.keysym.sym == SDLK_h)
		{
			window.clearPixels();
			rotateCamera(-THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_y)
		{
			window.clearPixels();
			rotateCamera(THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_t)
		{
			window.clearPixels();
			rotateCamera(0, 0, THETA);
		}
		else if (event.key.keysym.sym == SDLK_u)
		{
			window.clearPixels();
			rotateCamera(0, 0, -THETA);
		}
		else if (event.key.keysym.sym == SDLK_l)
		{
			window.clearPixels();
			lookCamera(vec3(0, 0, 0));
		}
		else if (event.key.keysym.sym == SDLK_LEFT)
		{
			window.clearPixels();
			orbitCamera(vec3(0, 0, -THETA));
		}
		else if (event.key.keysym.sym == SDLK_RIGHT)
		{
			window.clearPixels();
			orbitCamera(vec3(0, 0, THETA));
		}
		else if (event.key.keysym.sym == SDLK_UP)
		{
			window.clearPixels();
			orbitCamera(vec3(0, THETA, 0));
		}
		else if (event.key.keysym.sym == SDLK_DOWN)
		{
			window.clearPixels();
			orbitCamera(vec3(0, -THETA, 0));
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		// cout << "MOUSE CLICKED" << endl;
	}
}