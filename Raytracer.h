#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "Camera.h"

using namespace std;
using namespace glm;

RayTriangleIntersection getClosestIntersection(ObjContent obj, vec3 ray);
void drawRaytraces(ObjContent obj);

RayTriangleIntersection getClosestIntersection(ObjContent obj, vec3 ray)
{
	RayTriangleIntersection closest = RayTriangleIntersection();
	closest.distanceFromCamera = INFINITY;
	for (uint c = 0; c < obj.faces.size(); c++)
	{
		ModelTriangle triangle = obj.faces[c];

		vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
		vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
		vec3 SPVector = (CAMERA_POS - triangle.vertices[0]);
		mat3 DEMatrix(-ray, e0, e1);
		vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

		float t = abs(possibleSolution.x);
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

void drawRaytrace(ObjContent obj)
{
	for (int x = 0; x <= WIDTH; x++)
	{
		for (int y = 0; y <= HEIGHT; y++)
		{
			// calculate direction vector
			float xp = -(x - w);
			float yp = (y - h);
			vec3 ray = vec3(xp, yp, FOCAL_LENGTH) * glm::inverse(CAMERA_ROT);
			ray = glm::normalize(ray);
			RayTriangleIntersection intersection = getClosestIntersection(obj, ray);
			if (intersection.distanceFromCamera < INFINITY)
			{
				window.setPixelColour(x, y, -0.5, intersection.intersectedTriangle.colour.pack());
			}
		}
	}
}
#endif