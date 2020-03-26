#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "Camera.h"

using namespace std;
using namespace glm;

vector<vec3> LIGHTS;
int SHADOW_MODE;

RayTriangleIntersection getClosestIntersection(ObjContent obj, vec3 ray);
void drawRaytraces(ObjContent obj);
vec3 getReflectedRay(vec3 planeNormal, vec3 viewRay);

int countShadows(ObjContent obj, vector<vec3> lightSources, vec3 pointInWorld, vec3 ray, uint triangleIndex)
{
	int lightsInShadowOf = 0;
#pragma omp parallel
#pragma omp for
	// 18.07
	for (uint l = 0; l < lightSources.size(); l++)
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
			if (0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0 && (u + v) <= 1.0 && t > 0.4f && c != triangleIndex)
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

float getShadowProportionForSoftShadows(ObjContent obj, vector<vec3> lightSources, vec3 planeNormal, vec3 pointInWorld, vec3 ray, int triangleIndex)
{
	float totalShift = 0.1f;
	vec3 shift = totalShift * normalize(planeNormal);
	uint shadows = 0;
	// shift virtual plane up
	shadows += countShadows(obj, LIGHTS, pointInWorld + shift, ray, triangleIndex);
	// shift virtual plane down
	shadows += countShadows(obj, LIGHTS, pointInWorld - shift, ray, triangleIndex);
	shadows += countShadows(obj, LIGHTS, pointInWorld, ray, triangleIndex);
	float shadeProportion = 0.0f;
	// all levels in shade (lowest brightness)
	if (shadows == (lightSources.size() * 3))
	{
		shadeProportion = 1.0f;
	}
	// no levels in shade (dont change brightness)
	else if (shadows == 0)
	{
		shadeProportion = 0.0f;
	}
	// some levels in shade (calculate proportion of shadow)
	else
	{
		float thisShift = -1.0f * totalShift;
		// increment shift until shadows = 0
		while (shadows >= 1)
		{
			thisShift += 0.02f;
			shadows = countShadows(obj, LIGHTS, pointInWorld + (thisShift * normalize(planeNormal)), ray, triangleIndex);
		}
		// find value of shade between 0 -> light and 1 -> dark
		shadeProportion = (thisShift + totalShift) / (totalShift * 2.0f);
	}
	return shadeProportion;
}
bool isApproxEqual(float x, float y)
{
	const float epsilon = 0.01f;
	return abs(x - y) <= epsilon * abs(x);
}

bool getRefractedRay(vec3 planeNormal, vec3 viewRay, float surfaceRefractiveIndex, vec3 &refracted_ray)
{
	float viewDotNorm = dot(viewRay, planeNormal);
	float cosi = glm::clamp(-1.f, 1.f, dot(planeNormal, viewRay));
	float air = 1.0f;
	float refOut = air;
	float refIn = surfaceRefractiveIndex;
	// we are inside the surface
	if (viewDotNorm < 0)
	{
		viewDotNorm *= -1.0f;
	}
	// we are outside the surface
	else
	{
		planeNormal *= -1.0f;
		swap(refIn, refOut);
	}
	// n_1 / n_2
	float eta = refIn / refOut;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	if (k < 0)
	{
		// printf("reflected!\n");
		// return getReflectedRay(planeNormal, viewRay);
		return false;
	}
	else
	{
		refracted_ray = eta * viewRay + (eta * cosi - sqrtf(k)) * planeNormal;
		refracted_ray = normalize(refracted_ray);
		// printf("ray in = %f,%f,%f\n", viewRay.x, viewRay.y, viewRay.z);
		// printf("refracted ray = %f,%f,%f\n", refracted_ray.x, refracted_ray.y, refracted_ray.z);
		// printf("\n");
		return true;
	}
}

vec3 getReflectedRay(vec3 planeNormal, vec3 viewRay)
{
	planeNormal = normalize(planeNormal);
	if (dot(viewRay, planeNormal) > 0.0f)
	{
		planeNormal = -1.f * planeNormal;
	}
	else
	{
		viewRay = -1.f * viewRay;
	}
	vec3 reflection = viewRay - (2.0f * dot(planeNormal, viewRay) * planeNormal);
	return normalize(reflection);
}

float getBrightness(vector<vec3> lightSources, vec3 planeNormal, vec3 point_in_world, vec3 ray)
{
	float brightnessIncrease = 0.0f;
	for (uint i = 0; i < lightSources.size(); i++)
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
}

RayTriangleIntersection getClosestIntersection(ObjContent obj, vec3 ray, vec3 startPos, int ray_bounces_remaining)
{
	RayTriangleIntersection closest = RayTriangleIntersection();
	closest.distanceFromCamera = INFINITY;

	// if (ray_bounces_remaining == 0)
	// {
	// 	return RayTriangleIntersection(vec3(1, 1, 1), 1, obj.faces[0], Colour(0, 0, 0));
	// }
#pragma omp parallel
#pragma omp for
	for (uint c = 0; c < obj.faces.size(); c++)
	{
		ModelTriangle triangle = obj.faces[c];

		vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
		vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
		vec3 SPVector = (startPos - triangle.vertices[0]);
		mat3 DEMatrix(-ray, e0, e1);
		vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

		float t = abs(possibleSolution.x);
		float u = possibleSolution.y;
		float v = possibleSolution.z;
		// t,u,v is valid
		if (0.0f <= u && u <= 1.0f && 0.0f <= v && v <= 1.0f && (u + v) <= 1.0f && t >= 0.0001f)
		{
			if (t < closest.distanceFromCamera)
			{
				// find position on triangle
				vec3 u_tri = u * (triangle.vertices[1] - triangle.vertices[0]);
				vec3 v_tri = v * (triangle.vertices[2] - triangle.vertices[0]);
				vec3 point_world = (u_tri + v_tri) + triangle.vertices[0];

				vec3 planeNorm = cross(e0, e1);
				// if (dot(ray, planeNorm) > 0)
				// {
				// 	planeNorm = planeNorm * -1.0f;
				// }

				Colour col;
				float brightness = getBrightness(LIGHTS, planeNorm, point_world, ray);
				// refract!
				if (!(isApproxEqual(triangle.colour.refractivity, 1.0f)) && ray_bounces_remaining > 0)
				{
					// printf("%i\n", ray_bounces_remaining);
					vec3 refracted_ray;
					if (getRefractedRay(planeNorm, ray, triangle.colour.refractivity, refracted_ray))
					{
						RayTriangleIntersection intersection = getClosestIntersection(obj, refracted_ray, point_world, ray_bounces_remaining - 1);
						if (intersection.distanceFromCamera < INFINITY)
						{
							col = intersection.colour;
							intersection.colour.setBrightness(intersection.colour.brightness);
						}
						else
						{
							col = Colour(0, 0, 0);
						}
					}
					else
					{
						col = Colour(120, 120, 120);
					}
				}
				// reflect
				else if (triangle.colour.mirrorness > 0.01f && ray_bounces_remaining > 0)
				{
					vec3 reflected_ray = getReflectedRay(planeNorm, ray);
					// printf("%i\n", ray_bounces_remaining);
					RayTriangleIntersection intersection = getClosestIntersection(obj, reflected_ray, point_world, ray_bounces_remaining - 1);
					if (intersection.distanceFromCamera < INFINITY)
					{
						col = intersection.colour;
						intersection.colour.setBrightness(triangle.colour.mirrorness * intersection.colour.brightness);
					}
					else
					{
						col = Colour(0, 0, 0);
					}
				}
				else
				{
					if (brightness > 1.0f)
					{
						brightness = 1.0f;
					}
					// hard shadows
					if (SHADOW_MODE == 2)
					{
						uint shadows = countShadows(obj, LIGHTS, point_world, ray, c);
						if (shadows == LIGHTS.size())
						{
							brightness = 0.2f;
						}
					}
					// soft shadows
					else if (SHADOW_MODE == 3)
					{
						float shadeProportion = getShadowProportionForSoftShadows(obj, LIGHTS, planeNorm, point_world, ray, c);
						// TODO this can be tuned to be prettier
						brightness -= pow(shadeProportion, 0.5f);
					}
					col = Colour(triangle.colour.red, triangle.colour.green, triangle.colour.blue, brightness);
				}
				closest = RayTriangleIntersection(point_world, t, triangle, col);
			}
		}
	}
	return closest;
}

vec3 int32ToVec3(uint32_t rgb)
{
	int r = (rgb >> 16) & 0xFF;
	int g = (rgb >> 8) & 0xFF;
	int b = rgb & 0xFF;
	return vec3(r, g, b);
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
#pragma omp parallel
#pragma omp for
	for (int x = 0; x <= WIDTH; x++)
	{
		for (int y = 0; y <= HEIGHT; y++)
		{
			// calculate direction vector
			float xp = -(x - w);
			float yp = (y - h);
			vec3 ray = vec3(xp, yp, FOCAL_LENGTH) * glm::inverse(CAMERA_ROT);
			ray = glm::normalize(ray);
			RayTriangleIntersection intersection = getClosestIntersection(obj, ray, CAMERA_POS, 3);
			if (intersection.distanceFromCamera < INFINITY)
			{
				window.setPixelColour(x, y, -0.5, intersection.colour.pack());
			}
		}
	}
}
void drawRaytraceWithAA(ObjContent obj, int mode)
{

	vector<vec2> alias_pattern;
	// corners + middle
	alias_pattern.push_back(vec2(0.0f, 0.0f));
	alias_pattern.push_back(vec2(0.5f, 0.0f));
	alias_pattern.push_back(vec2(-0.5f, 0.0f));
	alias_pattern.push_back(vec2(0.0f, 0.5f));
	alias_pattern.push_back(vec2(0.0f, -0.5f));
	// float foo = 10.0f;
	// alias_pattern.push_back(vec2(2.0f / foo, 2.0f / foo));
	// alias_pattern.push_back(vec2(7.0f / foo, 1.0f / foo));
	// alias_pattern.push_back(vec2(3.0f / foo, 7.0f / foo));
	// alias_pattern.push_back(vec2(8.0f / foo, 6.0f / foo));

	vec3 a = vec3(-0.884011, 5.219334, -2.517968);
	vec3 b = vec3(0.415989, 5.218497, -3.567968);
	vector<vec3> empty;
	LIGHTS = empty;
	SHADOW_MODE = mode;
	LIGHTS.push_back(a + ((glm::length(a - b) / 3) * -(a - b)));
#pragma omp parallel
#pragma omp for
	for (int x = 0; x <= WIDTH; x++)
	{
		for (int y = 0; y <= HEIGHT; y++)
		{
			float xp = -(x - w);
			float yp = (y - h);
			vector<Colour> colours;
			vec3 ray;
			RayTriangleIntersection intersection;
			// for each subpixel in the pattern
			for (uint a = 0; a < alias_pattern.size(); a++)
			{
				// get ray and find colour
				ray = vec3(xp + alias_pattern.at(a).x, yp + alias_pattern.at(a).y, FOCAL_LENGTH) * glm::inverse(CAMERA_ROT);
				ray = glm::normalize(ray);
				intersection = getClosestIntersection(obj, ray, CAMERA_POS, 3);
				if (intersection.distanceFromCamera < INFINITY)
				{
					colours.push_back(intersection.colour);
				}
			}
			// get average of all colours
			if (colours.size() > 0)
			{
				Colour avgCol = colours.at(0);
				for (uint i = 1; i < colours.size(); i++)
				{
					Colour avgCol_prime = avgCol.averageWith(colours.at(i));
					avgCol = avgCol_prime;
				}
				window.setPixelColour(x, y, -0.5, avgCol.pack());
			}
		}
	}
}
#endif