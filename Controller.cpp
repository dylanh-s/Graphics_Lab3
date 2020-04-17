#include "Rasterizer.h"
#include "Raytracer.h"

using namespace std;
using namespace glm;

void update(int n);
void draw(OBJ obj);
void handleEvent(SDL_Event event);

void translateFaces(float x);

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
		// Uncomment for spinning logo
		// if (n % 10 == 1)
		// {	
		// 	vector<int> f;
		// 	for (uint i = 0; i < obj.faces.size(); i++) // create index array of faces to pass, 
		// 		f.push_back(i);							// so we can animate them separately
		// 	obj.rotateOBJ(0.1, 0.1, 0.1, f); // obj.translateOBJ(25, 0.0, 0.0, f);
		// }
	}
}

void update(int n)
{
	// cout << "frame" << n << "\n";
	if (n == 0)
	{
		mode = 0;
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
	window.clearPixels();
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
			mode = (mode + 1) % 6;
		}
		else if (event.key.keysym.sym == SDLK_n)
		{
			mode = (mode - 1) % 6;
		}
		else if (event.key.keysym.sym == SDLK_a)
		{
			translateCamera(DELTA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_d)
		{
			translateCamera(-DELTA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_s)
		{
			translateCamera(0, DELTA, 0);
		}
		else if (event.key.keysym.sym == SDLK_w)
		{
			translateCamera(0, -DELTA, 0);
		}
		else if (event.key.keysym.sym == SDLK_q)
		{
			translateCamera(0, 0, DELTA);
		}
		else if (event.key.keysym.sym == SDLK_e)
		{
			translateCamera(0, 0, -DELTA);
		}
		else if (event.key.keysym.sym == SDLK_f)
		{
			rotateCamera(0, THETA, 0);
		}
		else if (event.key.keysym.sym == SDLK_h)
		{
			rotateCamera(0, -THETA, 0);
		}
		else if (event.key.keysym.sym == SDLK_g)
		{
			rotateCamera(THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_t)
		{
			rotateCamera(-THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_r)
		{
			rotateCamera(0, 0, THETA);
		}
		else if (event.key.keysym.sym == SDLK_y)
		{
			rotateCamera(0, 0, -THETA);
		}
		else if (event.key.keysym.sym == SDLK_j)
		{
			orbitCamera(vec3(0, 0, -THETA));
		}
		else if (event.key.keysym.sym == SDLK_l)
		{
			orbitCamera(vec3(0, 0, THETA));
		}
		else if (event.key.keysym.sym == SDLK_k)
		{
			orbitCamera(vec3(0, -THETA, 0));
		}
		else if (event.key.keysym.sym == SDLK_i)
		{
			orbitCamera(vec3(0, THETA, 0));
		}
		else if (event.key.keysym.sym == SDLK_o)
		{
			lookCamera(vec3(0, 0, 0));
		}
	}
}