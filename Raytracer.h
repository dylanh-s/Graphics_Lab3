#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "Camera.h"

using namespace std;
using namespace glm;

vector<vec3> LIGHTS;
int SHADOW_MODE;

RayTriangleIntersection getClosestIntersection(ObjContent obj, PpmContent ppm, vec3 ray);
void drawRaytraces(ObjContent obj);

vec3 uintToVec3(uint32_t rgb)
{
	int r = (rgb >> 16) & 0xFF;
	int g = (rgb >> 8) & 0xFF;
	int b = rgb & 0xFF;
	return vec3(r, g, b);
}

Colour getCorrespondingTextureCol(float u, float v, PpmContent ppm, CanvasTriangle texture_tri)
{
	vec2 v0 = vec2(texture_tri.vertices[0].x, texture_tri.vertices[0].y);
	vec2 v1 = vec2(texture_tri.vertices[1].x, texture_tri.vertices[1].y);
	vec2 v2 = vec2(texture_tri.vertices[2].x, texture_tri.vertices[2].y);

	vec2 u_tex = u * (v1 - v0);
	vec2 v_tex = v * (v2 - v0);

	vec2 point_texture = (u_tex + v_tex) + v0;
	// printf("point_texture = %f,%f\n", point_texture.x, point_texture.y);
	uint32_t uint_col = ppm.image[point_texture.x * PPM_WIDTH][point_texture.y * PPM_HEIGHT];
	vec3 col = uintToVec3(uint_col);
	return Colour(col.x, col.y, col.z, 1.0f);
}

int countShadows(ObjContent obj, vector<vec3> lightSources, vec3 pointInWorld, vec3 ray, int triangleIndex)
{
	int lightsInShadowOf = 0;
#pragma omp parallel
#pragma omp for
	for (int l = 0; l < lightSources.size(); l++)
	{
		vec3 shadowRay = lightSources.at(l) - pointInWorld;
		// shadowRay = normalize(shadowRay);
		float distanceToLight = length(shadowRay);
		bool inShadow = 0;
		for (uint c = 0; c < obj.faces.size(); c++)
		{
			ModelTriangle triangle = ModelTriangle(obj.faces[c].vertices[0], obj.faces[c].vertices[1], obj.faces[c].vertices[2], Material());

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
				// printf("%f\n", t);
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
	int shadows = 0;
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

vec3 getColourOfPoint(vector<vec3> lightSources, vec3 planeNormal, vec3 point_in_world, vec3 ray, vec3 Ka, vec3 Kd, vec3 Ks, float sExp)
{
	vec3 Ip = vec3(20.0, 20.0, 20.0); // intensity at point
	vec3 Ia = vec3(0.05, 0.05, 0.05); // ambient intensity
	vec3 I = vec3(1.0, 1.0, 1.0);	 // final intensity
	for (int i = 0; i < lightSources.size(); i++)
	{
		vec3 lightVec = (LIGHTS.at(i) - point_in_world);
		// float f = (1 / (4 * M_PI * length(lightVec) * length(lightVec)));

		// float f = 9 / (M_PI * length(lightVec));
		float f = 10 / length(lightVec);

		// ambient
		vec3 ambient = Ia * Ka;
		// cout << "ambient" << ambient << endl;

		// Lambert
		vec3 normal = normalize(planeNormal);
		float light_dot_normal = dot(normalize(lightVec), normal);
		if (light_dot_normal < 0.0f)
		{
			normal *= -1.f;
			light_dot_normal = dot(normalize(lightVec), normal);
		}

		vec3 lambert = f * Ip * Kd * light_dot_normal;
		// cout << "lambert" << lambert << endl;

		// Phong
		vec3 flipRay = -1.0f * ray;
		vec3 reflection = lightVec - (2.0f * (dot(lightVec, normalize(planeNormal)) * normalize(planeNormal)));
		float reflection_dot_view = dot(normalize(reflection), normalize(flipRay));
		vec3 phong;
		if (reflection_dot_view < 0.0f)
		{
			phong = vec3(0.0f, 0.0f, 0.0f);
		}
		else
		{
			printf("%f\n", reflection_dot_view);
			phong = f * Ip * Ks * pow(reflection_dot_view, sExp);
		}
		// cout << "phong" << phong << endl;

		I = I * (ambient + lambert + phong);
		if (I.x > 255.0f)
		{
			I.x = 255.0f;
		}
		if (I.y > 255.0f)
		{
			I.y = 255.0f;
		}
		if (I.z > 255.0f)
		{
			I.z = 255.0f;
		}
	}
	return I;
}

float getBrightness(vector<vec3> lightSources, vec3 planeNormal, vec3 point_in_world, vec3 ray)
{
	float brightnessIncrease;
	// f = 1/(4pi*d^2)
	// vec3 I = Ia * Ka + (f * Ip * Kd(dot(lightVec,plane))) + f*Ip*Ks*dot(reflection,flipRay)^Ni

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
#pragma omp parallel
#pragma omp for
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
				vec3 Ka;
				triangle.material.getKa(u, v, obj.textureTris[c], Ka);
				vec3 Kd;
				triangle.material.getKd(u, v, obj.textureTris[c], Kd);
				vec3 Ks;
				triangle.material.getKd(u, v, obj.textureTris[c], Ks);
				float sExp = triangle.material.specularExponent;

				float brightness = 1.0;
				// hard shadows
				// if (SHADOW_MODE == 69)
				vec3 col = getColourOfPoint(LIGHTS, planeNorm, point_world, ray, Ka, Kd, Ks, sExp);
				// cout << col << endl;
				Colour pixelCol;
				pixelCol = Colour(col.x, col.y, col.z, 1.0f);
				// if (SHADOW_MODE == 2)
				// {
				// 	int shadows = countShadows(obj, LIGHTS, point_world, ray, c);
				// 	if (shadows == LIGHTS.size())
				// 	{
				// 		pixelCol.brightness = 0.2f;
				// 	}
				// }
				// // soft shadows
				// else if (SHADOW_MODE == 3)
				// {
				// 	float shadeProportion = getShadowProportionForSoftShadows(obj, LIGHTS, planeNorm, point_world, ray, c);
				// 	// TODO this can be tuned to be prettier
				// 	brightness -= pow(shadeProportion, 0.5f);
				// 	pixelCol.brightness = brightness;
				// }
				closest = RayTriangleIntersection(point_world, t, triangle, pixelCol);
			}
		}
	}
	return closest;
}

void drawRaytrace(ObjContent obj, int mode)
{
	// vec3 a = vec3(-0.884011, 5.219334, -2.517968);
	// vec3 b = vec3(0.415989, 5.218497, -3.567968);
	vector<vec3> empty;
	LIGHTS = empty;
	SHADOW_MODE = mode;
	// LIGHTS.push_back(a + ((glm::length(a - b) / 3) * -(a - b)));
	// LIGHTS.push_back(vec3(0, 1, 1));
	LIGHTS.push_back(vec3(412.000000, 230.000000, 100.000000));
#pragma omp parallel
#pragma omp for
	for (int x = 0; x <= WIDTH; x++)
	{
		// printf("row %i\n", x);
		for (int y = 0; y <= HEIGHT; y++)
		{
			// printf("%i,\n", y);
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
			for (int a = 0; a < alias_pattern.size(); a++)
			{
				// get ray and find colour
				ray = vec3(xp + alias_pattern.at(a).x, yp + alias_pattern.at(a).y, FOCAL_LENGTH) * glm::inverse(CAMERA_ROT);
				ray = glm::normalize(ray);
				intersection = getClosestIntersection(obj, ray);
				if (intersection.distanceFromCamera < INFINITY)
				{
					colours.push_back(intersection.colour);
				}
			}
			// get average of all colours
			if (colours.size() > 0)
			{
				Colour avgCol = colours.at(0);
				for (int i = 1; i < colours.size(); i++)
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