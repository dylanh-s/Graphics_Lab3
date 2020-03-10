#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "Camera.h"

using namespace std;
using namespace glm;

vector<vec3> LIGHTS;
int SHADOW_MODE;

RayTriangleIntersection getClosestIntersection(ObjContent obj, vec3 ray);
void drawRaytraces(ObjContent obj);

int inShadow(ObjContent obj, vector<vec3> lightSources, vec3 pointInWorld, vec3 ray, int triangleIndex)
{
	int lightsInShadowOf = 0;
	for (int l = 0; l < lightSources.size(); l++)
	{
		vec3 shadowRay = lightSources.at(l) - pointInWorld;
		// shadowRay = normalize(shadowRay);
		float distanceToLight = length(shadowRay);
		bool inShadow = 0;
		for (uint c = 0; c < obj.faces.size(); c++)
		{
			ModelTriangle triangle = obj.faces[c];

			vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
			vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
			vec3 SPVector = (pointInWorld - triangle.vertices[0]);
			mat3 DEMatrix(-normalize(shadowRay), e0, e1);
			vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

			float t = possibleSolution.x;
			float u = possibleSolution.y;
			float v = possibleSolution.z;
			// t,u,v is valid
			if (0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0 && (u + v) <= 1.0 && t > 0.3f && c != triangleIndex)
			{
				if (t < (distanceToLight) && (abs(t - distanceToLight) > 0.01f))
				{
					inShadow = true;
					break;
				}
			}
		}
		if (inShadow)
		{
			lightsInShadowOf += 1;
		}
	}
	return lightsInShadowOf;
}

float getBrightness(vector<vec3> lightSources, vec3 planeNormal, vec3 point_in_world, vec3 ray)
{
	float brightnessIncrease;
	for (int i = 0; i < lightSources.size(); i++)
	{
		brightnessIncrease = 0.0f;
		vec3 lightVec = LIGHTS.at(i) - point_in_world;

		// proximity
		brightnessIncrease += (1 / (4 * M_PI * length(lightVec) * length(lightVec)));

		// incidence
		float angle_between = dot(normalize(lightVec), normalize(planeNormal));
		if (angle_between > 0.0f)
		{
			brightnessIncrease += (angle_between);
		}

		vec3 flipRay = -1.0f * ray;
		vec3 normal = normalize(planeNormal);
		vec3 reflection = lightVec - (2.0f * (dot(lightVec, normal)) * normal);
		angle_between = dot(normalize(reflection), normalize(flipRay));
		if (angle_between > 0.0f)
		{
			brightnessIncrease += pow(angle_between, 24.0f);
		}
	}
	return brightnessIncrease;
	// implement bottom val on colour

	// proximity lighting:
	// length of vector from light to object in 3D
	// length is not between 0 and 1 so needs scaling. 1/(4pi*dist**2)

	// incidence lighting
	// distance & angle
	// dot the normal and light but normalised (might need -ves)
	// check between 0-1 float
	// increm brightness again

	// specular highlighting
	// 𝑟=𝑑−2(𝑑⋅𝑛)𝑛
	// normal = normalize(normal)
	// reflection=incidence-2(dot(incidence,normal))*normal
	// brightness = dot(reflection,ray)
}

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
				vec3 u_tri = u * (triangle.vertices[1] - triangle.vertices[0]);
				vec3 v_tri = v * (triangle.vertices[2] - triangle.vertices[0]);
				vec3 point_world = (u_tri + v_tri) + triangle.vertices[0];

				vec3 planeNorm = cross(e0, e1);

				float brightness = getBrightness(LIGHTS, planeNorm, point_world, ray);
				// hard shadows
				if (SHADOW_MODE == 2)
				{
					int shadows = inShadow(obj, LIGHTS, point_world, ray, c);
					if (shadows == 1)
					{
						brightness = 0.2f;
					}
					// float darkness = (shadows / LIGHTS.size()) * 1.0f;
					// brightness -= darkness;
				}
				// soft shadows
				else if (SHADOW_MODE == 3)
				{
					float totalShift = 0.1f;
					vec3 shift = totalShift * normalize(planeNorm);
					int shadows = 0;

					shadows += inShadow(obj, LIGHTS, point_world + shift, ray, c);
					shadows += inShadow(obj, LIGHTS, point_world - shift, ray, c);
					shadows += inShadow(obj, LIGHTS, point_world, ray, c);

					brightness -= (shadows * 0.2f);
				}
				Colour col = Colour(triangle.colour.red, triangle.colour.green, triangle.colour.blue, brightness);
				closest = RayTriangleIntersection(point_world, t, triangle, col);
			}
		}
	}
	return closest;
}

void drawRaytrace(ObjContent obj, int mode)
{
	vec3 a = vec3(-0.884011, 5.219334, -2.517968);
	vec3 b = vec3(0.415989, 5.218497, -3.567968);
	vector<vec3> empty;
	LIGHTS = empty;
	SHADOW_MODE = mode;
	LIGHTS.push_back(a + ((glm::length(a - b) / 3) * -(a - b)));
	// LIGHTS.push_back(vec3(0, 1, 1));
	// LIGHTS.push_back(CAMERA_POS);
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
				window.setPixelColour(x, y, -0.5, intersection.colour.pack());
			}
		}
	}
}
#endif