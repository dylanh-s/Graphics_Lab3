#include "Rasterizer.h"
#include "Raytracer.h"

using namespace std;
using namespace glm;

void draw();
void update(int n);
void handleEvent(SDL_Event event);

int c = 0;
vector<vector<int>> components;
vector<vector<int>> xmovements;
vector<vector<int>> ymovements;
int main(int argc, char *argv[])
{	
	int n = 0;
	SDL_Event event;
	vector<int> component;
	// This creates index array of the different logo pieces
	for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 12; j++)
				component.push_back(j + i * 52);
			components.push_back(component);
			component.clear();
			for (int j = 12; j < 24; j++)
				component.push_back(j + i * 52);
			components.push_back(component);
			component.clear();
			for (int j = 24; j < 52; j++)
				component.push_back(j + i * 52);
			components.push_back(component);
			component.clear();
		}
	xmovements.push_back(vector<int> {0,0,0,0,0,0,50,0,0,0,0});
	ymovements.push_back(vector<int> {-100,-50,-50,-50,-50,-50,0,-50,-50,-50,-50});
	xmovements.push_back(vector<int> {0,0,0,-50,0,0,0,-50,0,0});
	ymovements.push_back(vector<int> {-50,-50,-50,0,-50,-50,-50,0,-50,-50,});
	xmovements.push_back(vector<int> {0,0,0,0,0,0,0,0,0,0});
	ymovements.push_back(vector<int> {-100,-50,-50,-50,-50,-50,-50,-50,-50,-50});
	xmovements.push_back(vector<int> {0,0,0,0,0,0,0,50,0,0,0,0,0});
	ymovements.push_back(vector<int> {-50,-50,-50,-50,-50,-50,-50,0,-50,-50,-50,-50,-50});
	xmovements.push_back(vector<int> {0,0,0,0,0,0,0,0,0,0,0,0,0,0,-50,0,-50});
	ymovements.push_back(vector<int> {-100,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,0,-50,0});
	xmovements.push_back(vector<int> {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
	ymovements.push_back(vector<int> {-100,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50});
	xmovements.push_back(vector<int> {0,0,0,0,0,0,0,0,0,0,0,0,50,0,50,50});
	ymovements.push_back(vector<int> {-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,0,-50,0,0});
	xmovements.push_back(vector<int> {0,0,0,0,0,0,0,0,0,0,0,50,0,50});
	ymovements.push_back(vector<int> {-100,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,0,-50,0});
	xmovements.push_back(vector<int> {0,0,0,0,0,0,0,0,0,0,50,0,50,50});
	ymovements.push_back(vector<int> {-100,-50,-50,-50,-50,-50,-50,-50,-50,-50,0,-50,0,0});

	// To chose starting frame change 121 to any number 0-121 
	// (121 will start at last frame with all the logo visible for testing shadows)
	for (; n < 121; n++)
		update(n);
	// To chose stopping frame change 121 to any number 1-122
	while (n < 122)
	{
		if (window.pollForInputEvents(&event))
			handleEvent(event);
		update(n);
		draw();
		window.renderFrame();
		ppmWrite(n);
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
	cout << "frame " << n << "\n";
	if (n == 0)
	{
		mode = 4;
		obj = objRead("./inputs/logo_4.obj");
		lights.push_back(vec3(0.0, 300.0, 800.0));
		obj.translateOBJ(-50, 600, 0, components[0]);
		obj.translateOBJ(100, 450, 0, components[1]);
		obj.translateOBJ(0, 600, 0, components[2]);
		obj.translateOBJ(-50, 650, 0, components[3]);
		obj.translateOBJ(100, 850, 0, components[4]);
		obj.translateOBJ(0, 850, 0, components[5]);
		obj.translateOBJ(-150, 700, 0, components[6]);
		obj.translateOBJ(-100, 700, 0, components[7]);
		obj.translateOBJ(-150, 650, 0, components[8]);
	}
	else if (n < 16)
	{
		obj.translateOBJ(xmovements[5][n - 1], ymovements[5][n - 1], 0, components[5]);
	}
	else if (n < 33)
	{
		obj.translateOBJ(xmovements[4][n - 16], ymovements[4][n - 16], 0, components[4]);
	}
	else if (n < 46)
	{
		obj.translateOBJ(xmovements[3][n - 33], ymovements[3][n - 33], 0, components[3]);
	}
	else if (n < 56)
	{
		obj.translateOBJ(xmovements[2][n - 46], ymovements[2][n - 46], 0, components[2]);
	}
	else if (n < 67)
	{
		obj.translateOBJ(xmovements[0][n - 56], ymovements[0][n - 56], 0, components[0]);
	}
	else if (n < 77)
	{
		obj.translateOBJ(xmovements[1][n - 67], ymovements[1][n - 67], 0, components[1]);
	}
	else if (n < 93)
	{
		obj.translateOBJ(xmovements[6][n - 77], ymovements[6][n - 77], 0, components[6]);
	}
	else if (n < 107)
	{
		obj.translateOBJ(xmovements[8][n - 93], ymovements[8][n - 93], 0, components[8]);
	}
	else if (n < 121)
	{
		obj.translateOBJ(xmovements[7][n - 107], ymovements[7][n - 107], 0, components[7]);
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
			obj.translateOBJ(-50, 0, 0, components[c]);
		}
		else if (event.key.keysym.sym == SDLK_RIGHT)
		{
			obj.translateOBJ(50, 0, 0, components[c]);
		}
		else if (event.key.keysym.sym == SDLK_DOWN)
		{
			obj.translateOBJ(0, -50, 0, components[c]);
		}
		else if (event.key.keysym.sym == SDLK_UP)
		{
			obj.translateOBJ(0, 50, 0, components[c]);
		}
		else if (event.key.keysym.sym == SDLK_c)
		{
			c = (c + 1) % 9;
		}
	}
}