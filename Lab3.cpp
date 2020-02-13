#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
// #include "ObjReader.h"
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>

using namespace std;
using namespace glm;

#define WIDTH 500
#define HEIGHT 500
#define FOCAL_LENGTH 250
// viewpoint = (0,0,0)
// image plane center = (0,0,FOCAL_LENGTH)
#define OBJ_SCALE 100

class ObjContent
{
public:
	ObjContent() {}
	std::vector<ModelTriangle> faces;
	std::unordered_map<std::string, Colour> palette;
	int max = 0;

	void addFace(ModelTriangle face)
	{
		faces.push_back(face);
	}

	void addColour(Colour col, std::string name)
	{
		palette[name] = col;
	}
};

void draw();
void update();
void drawLine(CanvasPoint start, CanvasPoint end, Colour colour);
void drawObj(ObjContent content);
void handleEvent(SDL_Event event);
void draw3DTriangle(ModelTriangle tri, ObjContent cont);
float *interpolate(double from, double to, int numberOfValues);
vec3 *interpolate(vec3 from, vec3 to, int numberOfValues);
void drawStrokedTriangle(CanvasTriangle tri);
ObjContent objFileRead(string filename);
ObjContent populatePalette(string filename);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

int main(int argc, char *argv[])
{
	SDL_Event event;

	ObjContent content = objFileRead("cornell-box.obj");

	while (true)
	{
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(&event))
			handleEvent(event);
		update();
		drawObj(content);
		draw();
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}

float *interpolate(double from, double to, int numberOfValues)
{
	float *toReturn = new float[numberOfValues];

	float inc = (to - from) / (numberOfValues - 1);
	for (int i = 0; i < numberOfValues; i++)
	{
		toReturn[i] = from + (i * inc);
		// printf(" %f, ", toReturn[i]);
	}

	return toReturn;
}

vec3 *interpolate(vec3 from, vec3 to, int numberOfValues)
{
	vec3 *toReturn = new vec3[numberOfValues];

	float *vars = interpolate(from.x, to.x, numberOfValues);
	for (int i = 0; i < numberOfValues; i++)
	{
		toReturn[i].x = vars[i];
	}

	vars = interpolate(from.y, to.y, numberOfValues);
	for (int i = 0; i < numberOfValues; i++)
	{
		toReturn[i].y = vars[i];
	}

	vars = interpolate(from.z, to.z, numberOfValues);
	for (int i = 0; i < numberOfValues; i++)
	{
		toReturn[i].z = vars[i];
	}

	return toReturn;
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
	CanvasTriangle toReturn = CanvasTriangle(v0, v1, v2, Colour(rand() % 255, rand() % 255, rand() % 255));
	// printf("triangle at (%f,%f), (%f,%f), (%f,%f)\n", v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
	return toReturn;
}

ModelTriangle scaleTriangle(ModelTriangle t)
{
	ModelTriangle toReturn;
	toReturn.colour = t.colour;
	float center_x = (t.vertices[0].x + t.vertices[1].x + t.vertices[2].x) / 3;
	float center_y = (t.vertices[0].y + t.vertices[1].y + t.vertices[2].y) / 3;
	float center_z = (t.vertices[0].z + t.vertices[1].z + t.vertices[2].z) / 3;
	for (int i = 0; i < 3; i++)
	{
		float X_new = center_x + ((t.vertices[i].x - center_x) * OBJ_SCALE);
		float Y_new = center_y + (t.vertices[i].y - center_y) * OBJ_SCALE;
		float Z_new = center_z + (t.vertices[i].z - center_z) * OBJ_SCALE;
		Z_new += 1000;
		toReturn.vertices[i] = vec3(X_new, Y_new, Z_new);
	}
	return toReturn;
};

CanvasTriangle *SplitTriangle(CanvasTriangle tri)
{
	CanvasTriangle *toReturn = new CanvasTriangle[2];
	CanvasPoint top;
	CanvasPoint mid;
	CanvasPoint bot;
	// swaps so [0] is always top, [1] middle, [2] bottom
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
		swap(tri.vertices[1], tri.vertices[0]);
	}
	float xDiff = tri.vertices[0].x - tri.vertices[2].x;
	float yDiff = tri.vertices[0].y - tri.vertices[2].y;
	float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;
	float currentY = tri.vertices[2].y;
	float count = 0.0;
	float x = 0.0;
	printf("%f, %f, %f \n", tri.vertices[0].y, tri.vertices[1].y, tri.vertices[2].y);
	while (currentY >= tri.vertices[1].y)
	{
		currentY = tri.vertices[2].y + (yStepSize * count);
		x = tri.vertices[2].x + (xStepSize * count);
		count++;
	}
	printf("pixel at (%f,%f). mid vertex is at (%f,%f).\n", round(x), round(tri.vertices[1].y), tri.vertices[1].x, tri.vertices[1].y);
	// window.setPixelColour(round(x), round(tri.vertices[1].y), vec3ToColour(vec3(255, 255, 255)));
	CanvasPoint rastPoint = CanvasPoint(round(x), tri.vertices[1].y);
	// window.setPixelColour(round(x - 1), round(currentY), vec3ToColour(vec3(255, 255, 255)));
	// window.setPixelColour(round(x + 1), round(currentY), vec3ToColour(vec3(255, 255, 255)));
	// drawLine(CanvasPoint(0, round(tri.vertices[1].y)), CanvasPoint(window.width, round(tri.vertices[1].y)), Colour(255, 255, 255));
	printf("0: %f,  1: %f, 2: %f\n", tri.vertices[0].y, tri.vertices[1].y, rastPoint.y);
	printf("0: %f,  1: %f, 2: %f\n", tri.vertices[1].y, rastPoint.y, tri.vertices[2].y);
	toReturn[0] = CanvasTriangle(tri.vertices[0], tri.vertices[1], rastPoint);
	toReturn[1] = CanvasTriangle(tri.vertices[2], rastPoint, tri.vertices[1]);

	return toReturn;
}

void draw()
{
	// drawLine(CanvasPoint(0, window.height / 2), CanvasPoint(window.width, window.height / 2), Colour(255, 255, 255));
}

void drawObj(ObjContent content)
{

	for (int i = 0; i < content.faces.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			ModelTriangle tri = content.faces[i];
			CanvasPoint A = CanvasPoint((tri.vertices[j].x / tri.vertices[j].z) * FOCAL_LENGTH, (tri.vertices[j].y / tri.vertices[j].z) * FOCAL_LENGTH, 1);
			if (fabs(A.x) > content.max)
			{
				content.max = A.x;
			}
			else if (fabs(A.y) > content.max)
			{
				content.max = A.y;
			}
		}
	}
	int c = 0;
	for (int i = 0; i < content.faces.size(); i++)
	{
		// printf("drawing triangle %i\n", i);
		draw3DTriangle(content.faces.at(i), content);
		c = i;
	}
	// printf("%i triangles drawn!", c);
}

void drawLine(CanvasPoint start, CanvasPoint end, Colour colour)
{
	// printf("line from (%f,%f) to (%f,%f)\n", start.x, start.y, end.x, end.y);
	// window.clearPixels();
	float xDiff = start.x - end.x;
	float yDiff = start.y - end.y;
	float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;
	for (float i = 0.0; i < numberOfSteps; i++)
	{
		float x = end.x + (xStepSize * i);
		float y = end.y + (yStepSize * i);
		window.setPixelColour(round(x), round(y), colour.toInt32());
	}
}

void drawStrokedTriangle(CanvasTriangle tri)
{
	drawLine(tri.vertices[0], tri.vertices[1], tri.colour);
	drawLine(tri.vertices[1], tri.vertices[2], tri.colour);
	drawLine(tri.vertices[2], tri.vertices[0], tri.colour);
}

float getXPoint(CanvasPoint from, CanvasPoint to, int y)
{
	return (from.x + (y - from.y) / (to.y - from.y) * (to.x - from.x));
}

void drawFullTriangle(CanvasTriangle tri)
{
	CanvasTriangle *tris = SplitTriangle(tri);
	tris[1].colour = Colour(255, 0, 0);
	for (int t = 0; t < 2; t++)
	{
		float xDiff = tris[t].vertices[0].x - tris[t].vertices[1].x;
		float yDiff = tris[t].vertices[0].y - tris[t].vertices[1].y;
		float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
		float xStepSize = xDiff / numberOfSteps;
		float yStepSize = yDiff / numberOfSteps;

		float y;
		float x;

		for (float i = 0.0; i < numberOfSteps; i++)
		{
			x = tris[t].vertices[1].x + (xStepSize * i);
			y = tris[t].vertices[1].y + (yStepSize * i);
			//find corresponding point on [0][2] to current x,y
			float x_2 = getXPoint(CanvasPoint(tris[t].vertices[0]), CanvasPoint(tris[t].vertices[2]), y);

			drawLine(CanvasPoint(round(x), y), CanvasPoint(round(x_2), y), tri.colour);
		}
		// drawStrokedTriangle(tris[t]);
	}

	delete[] tris;
}

CanvasPoint toImageCoords(CanvasPoint p)
{
	int w = window.width / 2;
	int h = window.height / 2;
	float x_prime = w + (p.x);
	float y_prime = h + (p.y);
	return CanvasPoint(x_prime, y_prime);
}

CanvasPoint project3DPoint(vec3 p)
{
	CanvasPoint A = CanvasPoint((p.x / p.z) * FOCAL_LENGTH, (p.y / p.z) * FOCAL_LENGTH, FOCAL_LENGTH);
	return A;
}

void draw3DTriangle(ModelTriangle tri, ObjContent cont)
{

	printf("triangle:\n");
	printf("A=%f,%f,%f\n", tri.vertices[0].x, tri.vertices[0].y, tri.vertices[0].z);
	printf("B=%f,%f,%f\n", tri.vertices[1].x, tri.vertices[1].y, tri.vertices[1].z);
	printf("C=%f,%f,%f\n\n", tri.vertices[2].x, tri.vertices[2].y, tri.vertices[2].z);

	CanvasPoint A = project3DPoint(tri.vertices[0]);
	CanvasPoint B = project3DPoint(tri.vertices[1]);
	CanvasPoint C = project3DPoint(tri.vertices[2]);
	printf("projected:\n");
	printf("A=%f,%f\n", A.x, A.y);
	printf("B=%f,%f\n", B.x, B.y);
	printf("C=%f,%f\n\n", C.x, C.y);
	// printf("B=%f,%f\n", B.x, B.y);
	// printf("C=%f,%f\n", C.x, C.y);
	printf("projected and translated to (0,0):\n");
	A = toImageCoords(A);
	B = toImageCoords(B);
	C = toImageCoords(C);
	printf("A=%f,%f\n", A.x, A.y);
	printf("B=%f,%f\n", B.x, B.y);
	printf("C=%f,%f\n\n", C.x, C.y);
	printf("--------------\n");
	drawStrokedTriangle(CanvasTriangle(A, B, C, tri.colour));
}

void imageread(string filename)
{
}

ObjContent populatePalette(string filename)
{
	ObjContent content = ObjContent();
	ofstream myfile;
	string line;
	Colour currentCol = Colour(0, 0, 0);
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}
	string key;
	while (std::getline(in, line))
	{
		if (line.size() > 4)
		{
			if (line.substr(0, 6) == "newmtl")
			{
				string *stuff = split(line, ' ');
				key = stuff[1];
				currentCol = content.palette[key];
			}
			else if (line.substr(0, 2) == "Kd")
			{
				string *stuff = split(line, ' ');
				int r = 255 * stod(stuff[1]);
				int g = 255 * stod(stuff[2]);
				int b = 255 * stod(stuff[3]);
				content.addColour(Colour(r, g, b), key);
			}
		}
	}
	return content;
}
ObjContent objFileRead(string filename)
{
	ObjContent toReturn = populatePalette("cornell-box.mtl");
	vector<vec3> vertices;
	string line;
	Colour currentCol = Colour(0, 0, 0);
	// printf("%i", toReturn.palette["White"].red);
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}
	while (std::getline(in, line))
	{
		//check v for vertices
		if (line.size() > 4)
		{
			if (line.substr(0, 6) == "usemtl")
			{
				string *stuff = split(line, ' ');
				string key = stuff[1];
				std::cout << key << std::endl;
				currentCol = toReturn.palette[key];
			}
			else if (line.substr(0, 2) == "v ")
			{
				string *stuff = split(line, ' ');
				double x = stod(stuff[1]);
				double y = stod(stuff[2]);
				double z = stod(stuff[3]);
				vertices.push_back(vec3(x, y, z));
			}
			//check for faces
			else if (line.substr(0, 2) == "f ")
			{
				int A, B, C; //to store vertex indices
				const char *chh = line.c_str();
				sscanf(chh, "f %i/ %i/ %i/", &A, &B, &C); //here it read the line start with f and store the corresponding values in the variables
				A--;
				B--;
				C--;
				// printf("tri A at %f,%f,%f\n", vertices.at(A).x, vertices.at(A).y, vertices.at(A).z);
				ModelTriangle t = ModelTriangle(vertices.at(A), vertices.at(B), vertices.at(C), currentCol);
				ModelTriangle scaled_t = scaleTriangle(t);
				toReturn.addFace(scaled_t);
			}
		}
	}
	// printf("%f", fabs(toReturn.max));
	return toReturn;
}

void help()
{
}

void update()
{
	// Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_LEFT)
			cout << "LEFT" << endl;
		else if (event.key.keysym.sym == SDLK_RIGHT)
			cout << "RIGHT" << endl;
		else if (event.key.keysym.sym == SDLK_UP)
			cout << "UP" << endl;
		else if (event.key.keysym.sym == SDLK_DOWN)
			cout << "DOWN" << endl;
		else if (event.key.keysym.sym == SDLK_u)
		{
			cout << "U" << endl;
		}
		else if (event.key.keysym.sym == SDLK_c)
		{
			cout << "C" << endl;
			window.clearPixels();
		}
		else if (event.key.keysym.sym == SDLK_i)
		{
			imageread("");
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
		cout << "MOUSE CLICKED" << endl;
}