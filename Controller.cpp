#include "Rasterizer.h"
#include "Raytracer.h"

using namespace std;
using namespace glm;

void draw();
void update(int n);
void handleEvent(SDL_Event event);

int c = 0;
vector<vector<int>> components;
int main(int argc, char *argv[])
{	
	int n = 0;
	SDL_Event event;
	vector<int> component;
	for (int i = 0; i < 1; i++)
		{
			for (int j = 0; j < 156; j++)
				component.push_back(j + i * 52);
			components.push_back(component);
			component.clear();
		}
	while (true)
	{
		if (window.pollForInputEvents(&event))
			handleEvent(event);
		update(n);
		draw();
		window.renderFrame();
		n++;
	}
}

void draw()
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

void update(int n)
{
	// cout << "frame " << n << "\n";
	if (n == 0)
	{
		mode = 0;
		lights.push_back(vec3(0.0, 0.0, 200.0));
		obj = objRead("./inputs/logo_4.obj");
	}
	else if (n == 1)
	{
		ppmWrite(n);
	}
}

void handleEvent(SDL_Event event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_m)
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
			translateCamera(0, 0, -DELTA);
		}
		else if (event.key.keysym.sym == SDLK_e)
		{
			translateCamera(0, 0, DELTA);
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
			rotateCamera(-THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_t)
		{
			rotateCamera(THETA, 0, 0);
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
		else if (event.key.keysym.sym == SDLK_u)
		{
			orbitCamera(vec3(-DELTA, 0, 0));
		}
		else if (event.key.keysym.sym == SDLK_o)
		{
			orbitCamera(vec3(DELTA, 0, 0));
		}
		else if (event.key.keysym.sym == SDLK_SEMICOLON)
		{
			revolveCamera(0, -THETA, 0);
		}
		else if (event.key.keysym.sym == SDLK_HASH)
		{
			revolveCamera(0, THETA, 0);
		}
		else if (event.key.keysym.sym == SDLK_QUOTE)
		{
			revolveCamera(-THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_LEFTBRACKET)
		{
			revolveCamera(THETA, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_p)
		{
			revolveCamera(0, 0, -THETA);
		}
		else if (event.key.keysym.sym == SDLK_RIGHTBRACKET)
		{
			revolveCamera(0, 0, THETA);
		}
		else if (event.key.keysym.sym == SDLK_x)
		{
			lookCamera(vec3(0, 0, 0));
		}
		else if (event.key.keysym.sym == SDLK_z)
		{
			cameraPosition = vec3(0, 0, DEPTH);
			cameraRotation = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
		}
		else if (event.key.keysym.sym == SDLK_LEFT)
		{
			// obj.translateOBJ(-DELTA, 0, 0, components[c]);
			obj.rotateOBJ(-THETA, 0, 0, components[c]);
		}
		else if (event.key.keysym.sym == SDLK_RIGHT)
		{
			// obj.translateOBJ(DELTA, 0, 0, components[c]);
			obj.rotateOBJ(THETA, 0, 0, components[c]);
		}
		else if (event.key.keysym.sym == SDLK_DOWN)
		{
			// obj.translateOBJ(0, -DELTA, 0, components[c]);
			obj.rotateOBJ(0, -THETA, 0, components[c]);
		}
		else if (event.key.keysym.sym == SDLK_UP)
		{
			// obj.translateOBJ(0, DELTA, 0, components[c]);
			obj.rotateOBJ(0, THETA, 0, components[c]);
		}
		else if (event.key.keysym.sym == SDLK_c)
		{
			c = (c + 1) % 3;
		}
	}
}