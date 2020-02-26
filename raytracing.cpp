#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <RayTriangleIntersection.h>
// #include "ObjReader.h"
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>

using namespace std;
using namespace glm;

#define WIDTH 250
#define HEIGHT 250
#define STEP 1
float FOCAL_LENGTH = 125;
vec3 CAMERA_POS(0, 4, 5);
mat3 CAMERA_ROT(1, 0, 0, 0, 1, 0, 0, 0, 1);
bool stroked = false;
#define THETA 0.02
#define OBJ_SCALE 1000
#define OBJ_SHIFT 100

class PpmContent
{
public:
	PpmContent() {}
	std::vector<vector<uint32_t>> image;
	int col = 0;
	int row = 0;
};

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
// void draw3DTriangle(ModelTriangle tri, ObjContent cont);
// void drawFilledTriangle(CanvasTriangle tri);
// vector<double> interpolate(double from, double to, int numberOfValues);
// vector<vec3> interpolate3d(vec3 from, vec3 to, int numberOfValues);
// void drawStrokedTriangle(CanvasTriangle tri);
// CanvasTriangle getRandomTriangle();
ObjContent objFileRead(string filename);
void drawRaytraces(ObjContent cont);
ObjContent populatePalette(string filename);
// PpmContent ppmFileRead(string filename);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

int main(int argc, char *argv[])
{
	SDL_Event event;
	// PpmContent ppm = ppmFileRead("texture.ppm");
	ObjContent content = objFileRead("test-tri.obj");

	while (true)
	{
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(&event))
		{
			handleEvent(event);
		}
		// update();
		// drawFilledTriangle(getRandomTriangle());
		// drawObj(content);
		drawRaytraces(content);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}

RayTriangleIntersection getClosestIntersection(ObjContent cont, vec3 ray)
{
	RayTriangleIntersection closest = RayTriangleIntersection();
	closest.distanceFromCamera = INFINITY;
	for (int c = 0; c < cont.faces.size(); c++)
	{
		ModelTriangle triangle = cont.faces[c];

		vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
		vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
		vec3 SPVector = CAMERA_POS - triangle.vertices[0];
		mat3 DEMatrix(-ray, e0, e1);
		vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

		float t = possibleSolution.x;
		float u = possibleSolution.y;
		float v = possibleSolution.z;

		// t,u,v is valid
		if (0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0 && (u + v) <= 1.0)
		{
			if (t < closest.distanceFromCamera)
			{
				// find position on triangle
				vec3 u_tri = u * (triangle.vertices[2] - triangle.vertices[0]);
				vec3 v_tri = v * (triangle.vertices[1] - triangle.vertices[0]);
				vec3 point_world = (u_tri + v_tri) + triangle.vertices[0];
				closest = RayTriangleIntersection(point_world, t, triangle);
			}
		}
	}
	return closest;
}

CanvasPoint toImageCoords(CanvasPoint p)
{
	int w = window.width / 2;
	int h = window.height / 2;
	float xp = w + (p.x);
	float yp = h + (p.y);
	return CanvasPoint(xp, yp, p.depth);
}

CanvasPoint project3DPoint(vec3 p)
{
	p = ((p - CAMERA_POS)) * CAMERA_ROT;
	CanvasPoint A = CanvasPoint((p.x / p.z) * FOCAL_LENGTH, (p.y / p.z) * FOCAL_LENGTH, 1 / p.z);
	// printf("depth:  %f\n", p.z);
	return A;
}

void drawRaytraces(ObjContent cont)
{
	int w = window.width / 2;
	int h = window.height / 2;
	for (int x = 0; x <= WIDTH; x++)
	{
		for (int y = 0; y <= HEIGHT; y++)
		{
			// calculate direction vector
			float xp = (x)-w;
			float yp = (y)-h;
			vec3 ray = vec3(xp, yp, FOCAL_LENGTH);
			ray = glm::normalize(ray);
			// printf("ray (%f,%f) = [%f,%f,%f]\n", xp, yp, ray.x, ray.y, ray.z);
			RayTriangleIntersection intersection = getClosestIntersection(cont, ray);
			CanvasPoint intersection_point = toImageCoords(project3DPoint(intersection.intersectionPoint));
			window.setPixelColour(intersection_point.x, intersection_point.y, intersection_point.depth, intersection.intersectedTriangle.colour.pack());
		}
	}
}

std::vector<CanvasPoint> calculateLine(CanvasPoint start, CanvasPoint end)
{
	// printf("line from (%f,%f) to (%f,%f)\n", start.x, start.y, end.x, end.y);
	// window.clearPixels();

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
		points.push_back(CanvasPoint(x, y, z)); //convert this function to calculate line!
	}

	return points;
}

ObjContent populatePalette(string filename)
{
	ObjContent content;
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
				//std::cout << key << std::endl;
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
				// ModelTriangle scaled_t = scaleTriangle(t);
				toReturn.addFace(t);
			}
		}
	}
	// printf("%f", fabs(toReturn.max));
	return toReturn;
}

void drawLine(CanvasPoint start, CanvasPoint end, Colour colour)
{
	std::vector<CanvasPoint> points = calculateLine(start, end);
	for (uint i = 0; i < points.size(); i++)
	{
		// printf("point %f %f %f\n", points[i].x, points[i].y, 1 / points[i].depth);
		window.setPixelColour(points[i].x, points[i].y, points[i].depth, colour.pack());
	}
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
		else if (event.key.keysym.sym == SDLK_z)
		{
			window.clearPixels();
			stroked = !stroked;
		}
		else if (event.key.keysym.sym == SDLK_c)
		{
			cout << "C" << endl;
			window.clearPixels();
		}
		else if (event.key.keysym.sym == SDLK_i)
		{
			// ppmFileRead("");
		}
		else if (event.key.keysym.sym == SDLK_s)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z + STEP);
		}
		else if (event.key.keysym.sym == SDLK_w)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x, CAMERA_POS.y, CAMERA_POS.z - STEP);
		}
		else if (event.key.keysym.sym == SDLK_q)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x, CAMERA_POS.y + STEP, CAMERA_POS.z);
		}
		else if (event.key.keysym.sym == SDLK_e)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x, CAMERA_POS.y - STEP, CAMERA_POS.z);
		}
		else if (event.key.keysym.sym == SDLK_a)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x + STEP, CAMERA_POS.y, CAMERA_POS.z);
		}
		else if (event.key.keysym.sym == SDLK_d)
		{
			window.clearPixels();
			CAMERA_POS = vec3(CAMERA_POS.x - STEP, CAMERA_POS.y, CAMERA_POS.z);
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
			// lookAt(vec3(0.415989, 5.218497, -3.567968));
		}
		// else if (event.key.keysym.sym == SDLK_l)
		// {
		// 	cout << "C" << endl;
		// 	drawFilledTriangle(getRandomTriangle());
		// }
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		cout << "MOUSE CLICKED" << endl;
		// drawFilledTriangle(getRandomTriangle());
	}
}